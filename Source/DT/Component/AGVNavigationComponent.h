#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "AGVNavigationComponent.generated.h"

class ADTAGV;
class ADestinationPoint;

UENUM(BlueprintType)
enum class EAGVNavigationState : uint8
{
    Idle,
    Planning,
    Moving,
    Waiting,
    Arrived
};

USTRUCT(BlueprintType)
struct FWaypoint
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FVector Location;

    UPROPERTY(BlueprintReadOnly)
    float ArrivalRadius = 100.0f;

    UPROPERTY(BlueprintReadOnly)
    bool bStopAtWaypoint = false;

    FWaypoint() : Location(FVector::ZeroVector) {}
    FWaypoint(const FVector& InLocation) : Location(InLocation) {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNavigationComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaypointReached);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNavigationFailed, const FString&, Reason);

/**
 * AGV 네비게이션 컴포넌트
 * A* 기반 경로 찾기 및 장애물 회피
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DT_API UAGVNavigationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAGVNavigationComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "AGV Navigation")
    bool NavigateToDestination(const FGameplayTag& DestinationTag);

    UFUNCTION(BlueprintCallable, Category = "AGV Navigation")
    bool NavigateToLocation(const FVector& TargetLocation);

    UFUNCTION(BlueprintCallable, Category = "AGV Navigation")
    void StopNavigation();

    UFUNCTION(BlueprintPure, Category = "AGV Navigation")
    EAGVNavigationState GetNavigationState() const { return NavigationState; }

    UFUNCTION(BlueprintPure, Category = "AGV Navigation")
    float GetDistanceToDestination() const;

    UFUNCTION(BlueprintPure, Category = "AGV Navigation")
    FVector GetCurrentDestination() const { return CurrentDestination; }

    UPROPERTY(BlueprintAssignable, Category = "AGV Navigation")
    FOnNavigationComplete OnNavigationComplete;

    UPROPERTY(BlueprintAssignable, Category = "AGV Navigation")
    FOnWaypointReached OnWaypointReached;

    UPROPERTY(BlueprintAssignable, Category = "AGV Navigation")
    FOnNavigationFailed OnNavigationFailed;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
    float MovementSpeed = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
    float ArrivalDistance = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
    float ObstacleAvoidanceRadius = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
    float MaxPathDeviation = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
    bool bEnableObstacleAvoidance = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
    bool bEnableSmoothing = true;

private:
    UPROPERTY()
    TObjectPtr<ADTAGV> OwnerAGV;

    EAGVNavigationState NavigationState;
    FVector CurrentDestination;
    TArray<FWaypoint> PathWaypoints;
    int32 CurrentWaypointIndex;

    void UpdateMovement(float DeltaTime);
    void MoveAlongPath(float DeltaTime);
    bool GeneratePath(const FVector& Start, const FVector& End);
    void SmoothPath();
    bool CheckObstacles(const FVector& Direction, FVector& AvoidanceDirection);
    void OnDestinationReached();
    FVector GetSteeringDirection(float DeltaTime);
};