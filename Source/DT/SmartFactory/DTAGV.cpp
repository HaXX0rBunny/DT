
#include "SmartFactory/DTAGV.h"
#include "GASCore/AMR_Attributes.h"
#include "SmartFactory/Spec.h"
#include "Component/AGVNavigationComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Tags/DTGameplayTags.h"
#include "Instance/DT_DataManager.h"
#include "Engine/World.h"
#include "TimerManager.h"

ADTAGV::ADTAGV()
{
    AMRAttributes = CreateDefaultSubobject<UAMR_Attributes>(TEXT("AMRAttributes"));
    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
    NavigationComponent = CreateDefaultSubobject<UAGVNavigationComponent>(TEXT("NavigationComponent"));

    if (MovementComponent)
    {
        MovementComponent->MaxSpeed = MovementSpeed;
    }
}

void ADTAGV::BeginPlay()
{
    Super::BeginPlay();

    if (AbilitySystemComponent && AMRAttributes)
    {
        AbilitySystemComponent->AddAttributeSetSubobject(AMRAttributes.Get());
        AbilitySystemComponent->InitStats(UAMR_Attributes::StaticClass(), nullptr);
        AbilitySystemComponent->SetNumericAttributeBase(UAMR_Attributes::GetMaxBatteryLevelAttribute(), 100.0f);
        AbilitySystemComponent->SetNumericAttributeBase(UAMR_Attributes::GetBatteryLevelAttribute(), 100.0f);
        AbilitySystemComponent->SetNumericAttributeBase(UAMR_Attributes::GetCurrentLoadAttribute(), 0.0f);
    }

    const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->AddLooseGameplayTag(GameplayTags.Type_AGV);
        AbilitySystemComponent->AddLooseGameplayTag(GameplayTags.State_AGV_Idle);
    }

    if (NavigationComponent)
    {
        NavigationComponent->OnNavigationComplete.AddDynamic(this, &ADTAGV::OnNavigationComplete);
    }
}

void ADTAGV::AssignSpec(ASpec* NewSpec)
{
    if (!NewSpec || CurrentSpec)
    {
        return;
    }

    CurrentSpec = NewSpec;

    NewSpec->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SpecAttachSocketName);

    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
        ASC->AddLooseGameplayTag(GameplayTags.State_Busy);
        ASC->RemoveLooseGameplayTag(GameplayTags.State_Idle);
    }
}

void ADTAGV::ReleaseSpec()
{
    if (!CurrentSpec)
    {
        return;
    }

    CurrentSpec->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    CurrentSpec = nullptr;

    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
        ASC->RemoveLooseGameplayTag(GameplayTags.State_Busy);
        ASC->AddLooseGameplayTag(GameplayTags.State_Idle);
    }
}

bool ADTAGV::MoveToDestination(const FGameplayTag& DestinationTag)
{
    if (!NavigationComponent)
    {
        return false;
    }

    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
        ASC->AddLooseGameplayTag(GameplayTags.State_AGV_MoveToCell);
        ASC->RemoveLooseGameplayTag(GameplayTags.State_AGV_Idle);
    }

    return NavigationComponent->NavigateToDestination(DestinationTag);
}

bool ADTAGV::IsIdle() const
{
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
        return ASC->HasMatchingGameplayTag(GameplayTags.State_Idle);
    }
    return false;
}

void ADTAGV::OnNavigationComplete()
{
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();

        ASC->RemoveLooseGameplayTag(GameplayTags.State_AGV_MoveToCell);

        FGameplayEventData EventData;
        ASC->HandleGameplayEvent(GameplayTags.Event_AGV_MoveCompleted, &EventData);
    }
}