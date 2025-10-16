#include "SmartFactory/DTCollaborativeRobot.h"
#include "SmartFactory/Spec.h"
#include "GASCore/SpecAttributes.h"
#include "AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Tags/DTGameplayTags.h"
#include "Engine/World.h"
#include "TimerManager.h"

ADTCollaborativeRobot::ADTCollaborativeRobot()
{
    RobotMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RobotMesh"));
    RootComponent = RobotMesh;
}

void ADTCollaborativeRobot::BeginPlay()
{
    Super::BeginPlay();

    if (AbilitySystemComponent)
    {
        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
        AbilitySystemComponent->AddLooseGameplayTag(GameplayTags.State_CellRobot_Idle);
    }
}

void ADTCollaborativeRobot::PerformWorkOnSpec(ASpec* TargetSpec, const FGameplayTag& WorkType)
{
    if (!TargetSpec || IsWorking())
    {
        return;
    }

    if (!CanPerformWork(WorkType))
    {
        return;
    }

    CurrentWorkSpec = TargetSpec;

    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
        ASC->AddLooseGameplayTag(GameplayTags.State_CellRobot_Working);
        ASC->RemoveLooseGameplayTag(GameplayTags.State_CellRobot_Idle);
    }

    if (UAbilitySystemComponent* SpecASC = TargetSpec->GetAbilitySystemComponent())
    {
        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
        SpecASC->AddLooseGameplayTag(GameplayTags.State_Spec_WorkProgress);
        SpecASC->RemoveLooseGameplayTag(GameplayTags.State_Spec_Idle);
    }

    UWorld* World = GetWorld();
    if (World)
    {
        World->GetTimerManager().SetTimer(
            WorkTimerHandle,
            this,
            &ADTCollaborativeRobot::CompleteWork,
            WorkDuration,
            false
        );
    }
}

bool ADTCollaborativeRobot::CanPerformWork(const FGameplayTag& WorkType) const
{
    return SupportedWorkTypes.Contains(WorkType);
}

bool ADTCollaborativeRobot::IsWorking() const
{
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
        return ASC->HasMatchingGameplayTag(GameplayTags.State_CellRobot_Working);
    }
    return false;
}

void ADTCollaborativeRobot::CompleteWork()
{
    if (!CurrentWorkSpec)
    {
        return;
    }

    if (UAbilitySystemComponent* SpecASC = CurrentWorkSpec->GetAbilitySystemComponent())
    {
        const float CurrentProgress = SpecASC->GetNumericAttribute(USpecAttributes::GetWorkProgressAttribute());
        const float NewProgress = FMath::Min(CurrentProgress + WorkProgressPerCycle, 100.0f);

        SpecASC->SetNumericAttributeBase(USpecAttributes::GetWorkProgressAttribute(), NewProgress);

        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();

        if (NewProgress >= 100.0f)
        {
            SpecASC->AddLooseGameplayTag(GameplayTags.State_Spec_WorkComplete);
            SpecASC->RemoveLooseGameplayTag(GameplayTags.State_Spec_WorkProgress);
        }
    }

    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
        ASC->RemoveLooseGameplayTag(GameplayTags.State_CellRobot_Working);
        ASC->AddLooseGameplayTag(GameplayTags.State_CellRobot_Idle);

        FGameplayEventData EventData;
        ASC->HandleGameplayEvent(GameplayTags.Event_Task_Complete, &EventData);
    }

    CurrentWorkSpec = nullptr;
}