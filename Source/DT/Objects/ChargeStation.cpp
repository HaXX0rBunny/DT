#include "ChargeStation.h"
#include "SmartFactory/DTAGV.h"
#include "GASCore/AMR_Attributes.h"
#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

AChargeStation::AChargeStation()
{
    PrimaryActorTick.bCanEverTick = true;

    RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = RootComp;

    StationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StationMesh"));
    StationMesh->SetupAttachment(RootComponent);

    ChargingZone = CreateDefaultSubobject<UBoxComponent>(TEXT("ChargingZone"));
    ChargingZone->SetupAttachment(RootComponent);
    ChargingZone->SetBoxExtent(FVector(150.0f, 150.0f, 100.0f));
    ChargingZone->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    ChargingZone->SetGenerateOverlapEvents(true);
}

void AChargeStation::BeginPlay()
{
    Super::BeginPlay();

    ChargingZone->OnComponentBeginOverlap.AddDynamic(this, &AChargeStation::OnChargingZoneBeginOverlap);
    ChargingZone->OnComponentEndOverlap.AddDynamic(this, &AChargeStation::OnChargingZoneEndOverlap);
}

void AChargeStation::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentChargingAGV)
    {
        UpdateCharging(DeltaTime);
    }
}

bool AChargeStation::StartCharging(ADTAGV* AGV)
{
    if (!AGV || CurrentChargingAGV != nullptr)
    {
        return false;
    }

    UAbilitySystemComponent* ASC = AGV->GetAbilitySystemComponent();
    if (!ASC)
    {
        return false;
    }

    CurrentChargingAGV = AGV;

    float CurrentBattery = ASC->GetNumericAttribute(UAMR_Attributes::GetBatteryLevelAttribute());
    OnChargingStarted.Broadcast(AGV, CurrentBattery);

    UE_LOG(LogTemp, Log, TEXT("ChargeStation: Started charging %s (Battery: %.1f%%)"), 
        *AGV->GetName(), CurrentBattery);

    return true;
}

void AChargeStation::StopCharging()
{
    if (!CurrentChargingAGV)
    {
        return;
    }

    UAbilitySystemComponent* ASC = CurrentChargingAGV->GetAbilitySystemComponent();
    if (ASC)
    {
        float FinalBattery = ASC->GetNumericAttribute(UAMR_Attributes::GetBatteryLevelAttribute());
        OnChargingComplete.Broadcast(CurrentChargingAGV, FinalBattery);

        UE_LOG(LogTemp, Log, TEXT("ChargeStation: Charging complete for %s (Battery: %.1f%%)"), 
            *CurrentChargingAGV->GetName(), FinalBattery);
    }

    CurrentChargingAGV = nullptr;
}

void AChargeStation::OnChargingZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bAutoStartCharging)
    {
        return;
    }

    ADTAGV* AGV = Cast<ADTAGV>(OtherActor);
    if (AGV && IsAvailable())
    {
        StartCharging(AGV);
    }
}

void AChargeStation::OnChargingZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ADTAGV* AGV = Cast<ADTAGV>(OtherActor);
    if (AGV == CurrentChargingAGV)
    {
        StopCharging();
    }
}

void AChargeStation::UpdateCharging(float DeltaTime)
{
    if (!CurrentChargingAGV)
    {
        return;
    }

    UAbilitySystemComponent* ASC = CurrentChargingAGV->GetAbilitySystemComponent();
    if (!ASC)
    {
        StopCharging();
        return;
    }

    float CurrentBattery = ASC->GetNumericAttribute(UAMR_Attributes::GetBatteryLevelAttribute());
    float MaxBattery = ASC->GetNumericAttribute(UAMR_Attributes::GetMaxBatteryLevelAttribute());

    if (CurrentBattery >= MaxBattery || IsAGVFullyCharged(CurrentChargingAGV))
    {
        StopCharging();
        return;
    }

    float ChargeAmount = ChargeRate * DeltaTime;
    float NewBattery = FMath::Min(CurrentBattery + ChargeAmount, MaxBattery);

    ASC->SetNumericAttributeBase(UAMR_Attributes::GetBatteryLevelAttribute(), NewBattery);
}

bool AChargeStation::IsAGVFullyCharged(ADTAGV* AGV) const
{
    if (!AGV)
    {
        return false;
    }

    UAbilitySystemComponent* ASC = AGV->GetAbilitySystemComponent();
    if (!ASC)
    {
        return false;
    }

    float CurrentBattery = ASC->GetNumericAttribute(UAMR_Attributes::GetBatteryLevelAttribute());
    return CurrentBattery >= FullChargeThreshold;
}