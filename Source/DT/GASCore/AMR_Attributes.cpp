
#include "AMR_Attributes.h"
#include "Net/UnrealNetwork.h"

void UAMR_Attributes::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION_NOTIFY(UAMR_Attributes, BatteryLevel, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAMR_Attributes, MaxBatteryLevel, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAMR_Attributes, CurrentLoad, COND_None, REPNOTIFY_Always);
}

void UAMR_Attributes::OnRep_BatteryLevel(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAMR_Attributes, BatteryLevel, OldValue);
}

void UAMR_Attributes::OnRep_MaxBatteryLevel(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAMR_Attributes, MaxBatteryLevel, OldValue);
}

void UAMR_Attributes::OnRep_CurrentLoad(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAMR_Attributes, CurrentLoad, OldValue);
}