#include "SmartFactory/AMR_Robot.h"
#include "GASCore/AMR_Attributes.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/CapsuleComponent.h"

AAMR_Robot::AAMR_Robot()
{
    PrimaryActorTick.bCanEverTick = false;

    UCapsuleComponent* CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
    RootComponent = CapsuleComp;
    CapsuleComp->SetCapsuleSize(50.0f, 100.0f);
    CapsuleComp->SetCollisionProfileName(TEXT("Pawn"));

    AMRAttributes = CreateDefaultSubobject<UAMR_Attributes>(TEXT("AMRAttributes"));
    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));

    if (MovementComponent)
    {
        MovementComponent->MaxSpeed = 400.0f;
        MovementComponent->Acceleration = 800.0f;
        MovementComponent->Deceleration = 1000.0f;
    }

    if (AbilitySystemComponent && AMRAttributes)
    {
        AbilitySystemComponent->AddAttributeSetSubobject(AMRAttributes.Get());
    }
}

void AAMR_Robot::BeginPlay()
{
    Super::BeginPlay();

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitStats(UAMR_Attributes::StaticClass(), nullptr);
        AbilitySystemComponent->SetNumericAttributeBase(UAMR_Attributes::GetMaxBatteryLevelAttribute(), 100.0f);
        AbilitySystemComponent->SetNumericAttributeBase(UAMR_Attributes::GetBatteryLevelAttribute(), 100.0f);
        AbilitySystemComponent->SetNumericAttributeBase(UAMR_Attributes::GetCurrentLoadAttribute(), 0.0f);
    }
}