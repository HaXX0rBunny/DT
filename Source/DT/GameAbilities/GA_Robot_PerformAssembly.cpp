


#include "GA_Robot_PerformAssembly.h"
#include "SmartFactory/DTCollaborativeRobot.h"
#include "SmartFactory/Spec.h"
#include "AbilitySystemComponent.h"
#include "Tags/DTGameplayTags.h"


UGA_Robot_PerformAssembly::UGA_Robot_PerformAssembly()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_Robot_PerformAssembly::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	ADTCollaborativeRobot* Robot = Cast<ADTCollaborativeRobot>(ActorInfo->AvatarActor.Get());
	if (!Robot)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	ASpec* TargetSpec = nullptr;
	if (TriggerEventData && IsValid(TriggerEventData->Target))
	{
		TargetSpec = const_cast<ASpec*>(Cast<ASpec>(TriggerEventData->Target.Get()));
	}
	if (!TargetSpec)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
}

void UGA_Robot_PerformAssembly::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	{
		const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();

		if (TaskCompleteHandle.IsValid())
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(GameplayTags.Event_Task_Complete)
				.Remove(TaskCompleteHandle);
			TaskCompleteHandle.Reset();
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Robot_PerformAssembly::OnAssemblyComplete(FGameplayEventData Payload)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}