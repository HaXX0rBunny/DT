#include "GASCore/DT_GameplayAbility.h"

UDT_GameplayAbility::UDT_GameplayAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    bRetriggerInstancedAbility = false;
}