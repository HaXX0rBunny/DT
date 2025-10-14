


#include "SmartFactory/FactoryEntityBase.h"
#include "SmartFactory/FactoryAttributeSet.h"
#include "AbilitySystemComponent.h"

AFactoryEntityBase::AFactoryEntityBase()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    FactoryAttributes = CreateDefaultSubobject<UFactoryAttributeSet>(TEXT("FactoryAttributes"));
}

UAbilitySystemComponent* AFactoryEntityBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}


