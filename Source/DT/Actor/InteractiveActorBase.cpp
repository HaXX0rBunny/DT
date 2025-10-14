


#include "InteractiveActorBase.h"
#include "AbilitySystemComponent.h"
#include "GASCore/FactoryObjectAttributes.h"

AInteractiveActorBase::AInteractiveActorBase()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    BaseAttributes = CreateDefaultSubobject<UFactoryObjectAttributes>(TEXT("BaseAttributes"));
}

UAbilitySystemComponent* AInteractiveActorBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}