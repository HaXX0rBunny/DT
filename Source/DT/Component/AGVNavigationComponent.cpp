#include "AGVNavigationComponent.h"
#include "SmartFactory/DTAGV.h"
#include "SmartFactory/DestinationPoint.h"
#include "Instance/DT_DataManager.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EngineUtils.h"

UAGVNavigationComponent::UAGVNavigationComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;

    NavigationState = EAGVNavigationState::Idle;
    CurrentWaypointIndex = 0;
}

void UAGVNavigationComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerAGV = Cast<ADTAGV>(GetOwner());
    if (!OwnerAGV)
    {
        UE_LOG(LogTemp, Error, TEXT("AGVNavigationComponent: Owner is not an ADTAGV!"));
    }
}

void UAGVNavigationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (NavigationState == EAGVNavigationState::Moving)
    {
        UpdateMovement(DeltaTime);
    }
}

bool UAGVNavigationComponent::NavigateToDestination(const FGameplayTag& DestinationTag)
{
    if (!GetWorld())
    {
        return false;
    }

    UGameInstance* GameInstance = GetWorld()->GetGameInstance();
    if (!GameInstance)
    {
        return false;
    }

    UDT_DataManager* DataManager = GameInstance->GetSubsystem<UDT_DataManager>();
    if (!DataManager)
    {
        return false;
    }

    FVector DestinationLocation;
    if (!DataManager->GetDestinationLocation(DestinationTag, DestinationLocation))
    {
        OnNavigationFailed.Broadcast(TEXT("Destination not found"));
        return false;
    }

    return NavigateToLocation(DestinationLocation);
}

bool UAGVNavigationComponent::NavigateToLocation(const FVector& TargetLocation)
{
    if (!OwnerAGV)
    {
        return false;
    }

    NavigationState = EAGVNavigationState::Planning;
    CurrentDestination = TargetLocation;

    FVector StartLocation = OwnerAGV->GetActorLocation();

    if (!GeneratePath(StartLocation, TargetLocation))
    {
        NavigationState = EAGVNavigationState::Idle;
        OnNavigationFailed.Broadcast(TEXT("Path generation failed"));
        return false;
    }

    if (bEnableSmoothing)
    {
        SmoothPath();
    }

    CurrentWaypointIndex = 0;
    NavigationState = EAGVNavigationState::Moving;

    UE_LOG(LogTemp, Log, TEXT("AGVNavigation: Started navigation to %s"), *TargetLocation.ToString());

    return true;
}

void UAGVNavigationComponent::StopNavigation()
{
    NavigationState = EAGVNavigationState::Idle;
    PathWaypoints.Empty();
    CurrentWaypointIndex = 0;

    if (OwnerAGV)
    {
        if (UFloatingPawnMovement* MovementComp = OwnerAGV->FindComponentByClass<UFloatingPawnMovement>())
        {
            MovementComp->StopMovementImmediately();
        }
    }
}

float UAGVNavigationComponent::GetDistanceToDestination() const
{
    if (!OwnerAGV || NavigationState == EAGVNavigationState::Idle)
    {
        return -1.0f;
    }

    return FVector::Dist(OwnerAGV->GetActorLocation(), CurrentDestination);
}

void UAGVNavigationComponent::UpdateMovement(float DeltaTime)
{
    if (!OwnerAGV || PathWaypoints.Num() == 0 || CurrentWaypointIndex >= PathWaypoints.Num())
    {
        return;
    }

    MoveAlongPath(DeltaTime);
}

void UAGVNavigationComponent::MoveAlongPath(float DeltaTime)
{
    const FWaypoint& CurrentWaypoint = PathWaypoints[CurrentWaypointIndex];
    FVector CurrentLocation = OwnerAGV->GetActorLocation();
    float DistanceToWaypoint = FVector::Dist2D(CurrentLocation, CurrentWaypoint.Location);

    if (DistanceToWaypoint <= CurrentWaypoint.ArrivalRadius)
    {
        CurrentWaypointIndex++;
        OnWaypointReached.Broadcast();

        if (CurrentWaypointIndex >= PathWaypoints.Num())
        {
            OnDestinationReached();
            return;
        }

        if (CurrentWaypoint.bStopAtWaypoint)
        {
            NavigationState = EAGVNavigationState::Waiting;
            return;
        }
    }

    FVector SteeringDirection = GetSteeringDirection(DeltaTime);

    if (UFloatingPawnMovement* MovementComp = OwnerAGV->FindComponentByClass<UFloatingPawnMovement>())
    {
        MovementComp->AddInputVector(SteeringDirection);
    }

    FRotator TargetRotation = SteeringDirection.Rotation();
    FRotator CurrentRotation = OwnerAGV->GetActorRotation();
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5.0f);
    OwnerAGV->SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));
}

bool UAGVNavigationComponent::GeneratePath(const FVector& Start, const FVector& End)
{
    PathWaypoints.Empty();

    FVector Direction = End - Start;
    Direction.Z = 0.0f;
    float TotalDistance = Direction.Size();

    if (TotalDistance < ArrivalDistance)
    {
        PathWaypoints.Add(FWaypoint(End));
        return true;
    }

    Direction.Normalize();

    int32 NumWaypoints = FMath::CeilToInt(TotalDistance / 500.0f);
    float StepSize = TotalDistance / NumWaypoints;

    for (int32 i = 1; i <= NumWaypoints; ++i)
    {
        FVector WaypointLocation = Start + Direction * (StepSize * i);
        WaypointLocation.Z = Start.Z;

        FWaypoint Waypoint(WaypointLocation);
        Waypoint.ArrivalRadius = (i == NumWaypoints) ? ArrivalDistance : 100.0f;
        PathWaypoints.Add(Waypoint);
    }

    return PathWaypoints.Num() > 0;
}

void UAGVNavigationComponent::SmoothPath()
{
    if (PathWaypoints.Num() < 3)
    {
        return;
    }

    TArray<FWaypoint> SmoothedPath;
    SmoothedPath.Add(PathWaypoints[0]);

    for (int32 i = 1; i < PathWaypoints.Num() - 1; ++i)
    {
        FVector PrevLoc = PathWaypoints[i - 1].Location;
        FVector CurrentLoc = PathWaypoints[i].Location;
        FVector NextLoc = PathWaypoints[i + 1].Location;

        FVector SmoothedLoc = (PrevLoc + CurrentLoc * 2.0f + NextLoc) * 0.25f;

        FWaypoint SmoothedWaypoint(SmoothedLoc);
        SmoothedWaypoint.ArrivalRadius = PathWaypoints[i].ArrivalRadius;
        SmoothedPath.Add(SmoothedWaypoint);
    }

    SmoothedPath.Add(PathWaypoints.Last());
    PathWaypoints = SmoothedPath;
}

bool UAGVNavigationComponent::CheckObstacles(const FVector& Direction, FVector& AvoidanceDirection)
{
    if (!bEnableObstacleAvoidance || !OwnerAGV)
    {
        return false;
    }

    FVector StartLocation = OwnerAGV->GetActorLocation();
    FVector EndLocation = StartLocation + Direction * ObstacleAvoidanceRadius;

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(OwnerAGV);

    FHitResult HitResult;
    bool bHit = UKismetSystemLibrary::SphereTraceSingle(
        GetWorld(),
        StartLocation,
        EndLocation,
        50.0f,
        UEngineTypes::ConvertToTraceType(ECC_Pawn),
        false,
        ActorsToIgnore,
        EDrawDebugTrace::None,
        HitResult,
        true
    );

    if (bHit)
    {
        FVector ToObstacle = HitResult.Location - StartLocation;
        ToObstacle.Z = 0.0f;
        ToObstacle.Normalize();

        FVector RightVector = FVector::CrossProduct(ToObstacle, FVector::UpVector);
        AvoidanceDirection = RightVector;

        return true;
    }

    return false;
}

void UAGVNavigationComponent::OnDestinationReached()
{
    NavigationState = EAGVNavigationState::Arrived;

    if (UFloatingPawnMovement* MovementComp = OwnerAGV->FindComponentByClass<UFloatingPawnMovement>())
    {
        MovementComp->StopMovementImmediately();
    }

    OnNavigationComplete.Broadcast();

    UE_LOG(LogTemp, Log, TEXT("AGVNavigation: Destination reached"));

    NavigationState = EAGVNavigationState::Idle;
    PathWaypoints.Empty();
}

FVector UAGVNavigationComponent::GetSteeringDirection(float DeltaTime)
{
    if (CurrentWaypointIndex >= PathWaypoints.Num())
    {
        return FVector::ZeroVector;
    }

    const FWaypoint& TargetWaypoint = PathWaypoints[CurrentWaypointIndex];
    FVector CurrentLocation = OwnerAGV->GetActorLocation();

    FVector DesiredDirection = TargetWaypoint.Location - CurrentLocation;
    DesiredDirection.Z = 0.0f;
    DesiredDirection.Normalize();

    FVector SteeringDirection = DesiredDirection;

    if (bEnableObstacleAvoidance)
    {
        FVector AvoidanceDirection;
        if (CheckObstacles(DesiredDirection, AvoidanceDirection))
        {
            SteeringDirection = (DesiredDirection + AvoidanceDirection * 0.5f).GetSafeNormal();
        }
    }

    return SteeringDirection;
}