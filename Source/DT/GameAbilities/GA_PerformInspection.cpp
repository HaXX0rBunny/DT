#include "GA_PerformInspection.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "SmartFactory/Spec.h"
#include "Tags/DTGameplayTags.h"

void UGA_PerformInspection::ActivateAbility(
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

    ASpec* TargetSpec = nullptr;
    if (TriggerEventData && IsValid(TriggerEventData->Target.Get()))
    {
        TargetSpec = const_cast<ASpec*>(Cast<ASpec>(TriggerEventData->Target.Get()));
    }

    if (!TargetSpec)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    PerformInspectionOnSpec(TargetSpec);
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_PerformInspection::PerformInspectionOnSpec(ASpec* Spec)
{
    if (!Spec)
    {
        return;
    }

    UAbilitySystemComponent* SpecASC = Spec->GetAbilitySystemComponent();
    if (!SpecASC)
    {
        return;
    }

    const bool bPassedInspection = FMath::RandRange(0.0f, 1.0f) >= DefectRate;

    const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();

    if (bPassedInspection)
    {
        SpecASC->AddLooseGameplayTag(GameplayTags.Product_Quality_Passed);
        SpecASC->RemoveLooseGameplayTag(GameplayTags.Product_Quality_Failed);
    }
    else
    {
        SpecASC->AddLooseGameplayTag(GameplayTags.Product_Quality_Failed);
        SpecASC->RemoveLooseGameplayTag(GameplayTags.Product_Quality_Passed);
    }

    SpecASC->AddLooseGameplayTag(GameplayTags.State_Spec_WorkComplete);
    SpecASC->RemoveLooseGameplayTag(GameplayTags.State_Spec_WorkProgress);
}