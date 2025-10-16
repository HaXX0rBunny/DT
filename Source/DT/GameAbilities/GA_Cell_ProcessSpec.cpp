#include "GA_Cell_ProcessSpec.h"
#include "SmartFactory/FactoryCell.h"
#include "SmartFactory/Spec.h"
#include "GASCore/SpecAttributes.h"
#include "GASCore/AssemblyLineAttributes.h"
#include "AbilitySystemComponent.h"
#include "Tags/DTGameplayTags.h"
#include "Engine/World.h"
#include "TimerManager.h"

UGA_Cell_ProcessSpec::UGA_Cell_ProcessSpec()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_Cell_ProcessSpec::ActivateAbility(
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

    AFactoryCell* Cell = Cast<AFactoryCell>(ActorInfo->AvatarActor.Get());
    if (!Cell)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    if (TriggerEventData && IsValid(TriggerEventData->Target))
    {
        CurrentSpec = const_cast<ASpec*>(Cast<ASpec>(TriggerEventData->Target.Get()));
    }

    if (!CurrentSpec)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    UpdateCellState(Cell, true);

    if (UAbilitySystemComponent* SpecASC = CurrentSpec->GetAbilitySystemComponent())
    {
        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
        SpecASC->AddLooseGameplayTag(GameplayTags.State_Spec_WorkProgress);
        SpecASC->RemoveLooseGameplayTag(GameplayTags.State_Spec_Idle);
    }

    UWorld* World = GetWorld();
    if (World)
    {
        World->GetTimerManager().SetTimer(
            ProcessTimerHandle,
            this,
            &UGA_Cell_ProcessSpec::ProcessComplete,
            ProcessDuration,
            false
        );
    }
}

void UGA_Cell_ProcessSpec::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    AFactoryCell* Cell = Cast<AFactoryCell>(ActorInfo->AvatarActor.Get());
    if (Cell)
    {
        UpdateCellState(Cell, false);
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(ProcessTimerHandle);
    }

    CurrentSpec = nullptr;

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Cell_ProcessSpec::ProcessComplete()
{
    if (!CurrentSpec)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    if (UAbilitySystemComponent* SpecASC = CurrentSpec->GetAbilitySystemComponent())
    {
        const float CurrentProgress = SpecASC->GetNumericAttribute(USpecAttributes::GetWorkProgressAttribute());
        const float NewProgress = FMath::Min(CurrentProgress + ProgressIncrement, 100.0f);

        SpecASC->SetNumericAttributeBase(USpecAttributes::GetWorkProgressAttribute(), NewProgress);

        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();

        if (NewProgress >= 100.0f)
        {
            SpecASC->AddLooseGameplayTag(GameplayTags.State_Spec_WorkComplete);
            SpecASC->RemoveLooseGameplayTag(GameplayTags.State_Spec_WorkProgress);
        }
    }

    if (UAbilitySystemComponent* CellASC = CurrentActorInfo->AbilitySystemComponent.Get())
    {
        CellASC->SetNumericAttributeBase(
            UAssemblyLineAttributes::GetProductionProgressAttribute(),
            100.0f
        );
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Cell_ProcessSpec::UpdateCellState(AFactoryCell* Cell, bool bIsProcessing)
{
    if (!Cell)
    {
        return;
    }

    UAbilitySystemComponent* ASC = Cell->GetAbilitySystemComponent();
    if (!ASC)
    {
        return;
    }

    const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();

    if (bIsProcessing)
    {
        ASC->AddLooseGameplayTag(GameplayTags.State_Cell_Progress);
        ASC->RemoveLooseGameplayTag(GameplayTags.State_Cell_Idle);

        ASC->SetNumericAttributeBase(UAssemblyLineAttributes::GetProductionProgressAttribute(), 0.0f);
    }
    else
    {
        ASC->AddLooseGameplayTag(GameplayTags.State_Cell_Complete);
        ASC->RemoveLooseGameplayTag(GameplayTags.State_Cell_Progress);

        ASC->SetNumericAttributeBase(UAssemblyLineAttributes::GetProductionProgressAttribute(), 100.0f);
    }
}