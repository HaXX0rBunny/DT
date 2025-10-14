

#include "AssemblyLineAttributes.h"
#include "Net/UnrealNetwork.h"

void UAssemblyLineAttributes::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION_NOTIFY(UAssemblyLineAttributes, ProductionProgress, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAssemblyLineAttributes, Efficiency, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAssemblyLineAttributes, WearAndTear, COND_None, REPNOTIFY_Always);
}

void UAssemblyLineAttributes::OnRep_ProductionProgress(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAssemblyLineAttributes, ProductionProgress, OldValue);
}

void UAssemblyLineAttributes::OnRep_Efficiency(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAssemblyLineAttributes, Efficiency, OldValue);
}

void UAssemblyLineAttributes::OnRep_WearAndTear(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAssemblyLineAttributes, WearAndTear, OldValue);
}