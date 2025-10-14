

#include "SpecAttributes.h"
#include "Net/UnrealNetwork.h"

void USpecAttributes::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION_NOTIFY(USpecAttributes, WorkProgress, COND_None, REPNOTIFY_Always);
}

void USpecAttributes::OnRep_WorkProgress(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(USpecAttributes, WorkProgress, OldValue);
}