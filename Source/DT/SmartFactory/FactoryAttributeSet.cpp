
#include "SmartFactory/FactoryAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UFactoryAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UFactoryAttributeSet, WearLevel, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UFactoryAttributeSet, CycleCount, COND_None, REPNOTIFY_Always);
}

void UFactoryAttributeSet::OnRep_WearLevel(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UFactoryAttributeSet, WearLevel, OldValue);
}

void UFactoryAttributeSet::OnRep_CycleCount(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UFactoryAttributeSet, CycleCount, OldValue);
}