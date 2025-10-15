
#include "GA_AGV_MoveToTarget.h"
#include "SmartFactory/DTAGV.h"
#include "AbilitySystemComponent.h"
#include "Tags/DTGameplayTags.h"


UGA_AGV_MoveToTarget::UGA_AGV_MoveToTarget()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_AGV_MoveToTarget::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
	ADTAGV* AGV = Cast<ADTAGV>(ActorInfo->AvatarActor.Get());
    if (!AGV)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }
    FGameplayTag DestinationTag = TargetDestinationTag;
	if (TriggerEventData && TriggerEventData->EventTag.IsValid())
		DestinationTag = TriggerEventData->EventTag;
    if (!DestinationTag.IsValid())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
    }
    if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
    {
		const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();

        MoveCompletedHandle = ASC->GenericGameplayEventCallbacks
            .FindOrAdd(GameplayTags.Event_AGV_MoveCompleted)
            .AddUObject(this, &UGA_AGV_MoveToTarget::OnMoveCompleted);
    }
	AGV->MoveToDestination(DestinationTag);
}

void UGA_AGV_MoveToTarget::EndAbility(
    const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, 
    bool bReplicateEndAbility, 
    bool bWasCancelled)
{
    if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
    {
		const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
        if (MoveCompletedHandle.IsValid())
        {
            ASC->GenericGameplayEventCallbacks.FindOrAdd(GameplayTags.Event_AGV_MoveCompleted)
			    .Remove(MoveCompletedHandle);
            MoveCompletedHandle.Reset();
        }
    }
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_AGV_MoveToTarget::OnMoveCompleted(const FGameplayEventData* Payload)
{
    if (Payload)
    {
        UE_LOG(LogTemp, Warning, TEXT("Move completed event received."));
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
}

