#include "Subsystem/AGVManagerSubsystem.h"
#include "SmartFactory/DTAGV.h"
#include "SmartFactory/Spec.h"
#include "AbilitySystemComponent.h"
#include "Tags/DTGameplayTags.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UAGVManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimerForNextTick(this, &UAGVManagerSubsystem::ScanForAGVs);

        World->GetTimerManager().SetTimer(
            UpdateTimerHandle,
            this,
            &UAGVManagerSubsystem::UpdateActiveTasks,
            1.0f,
            true
        );
    }
}

void UAGVManagerSubsystem::Deinitialize()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(UpdateTimerHandle);
    }

    RegisteredAGVs.Empty();
    ActiveTasks.Empty();

    Super::Deinitialize();
}

void UAGVManagerSubsystem::RegisterAGV(ADTAGV* AGV)
{
    if (!AGV || RegisteredAGVs.Contains(AGV))
    {
        return;
    }

    RegisteredAGVs.Add(AGV);

    if (UAbilitySystemComponent* ASC = AGV->GetAbilitySystemComponent())
    {
        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
        ASC->AddLooseGameplayTag(GameplayTags.State_AGV_Idle);
    }
}

ADTAGV* UAGVManagerSubsystem::GetIdleAGV()
{
    for (ADTAGV* AGV : RegisteredAGVs)
    {
        if (AGV && AGV->IsIdle() && !AGV->HasSpec())
        {
            return AGV;
        }
    }
    return nullptr;
}

bool UAGVManagerSubsystem::RequestAGVTransport(ASpec* Spec, const FGameplayTag& DestinationTag)
{
    if (!Spec || !DestinationTag.IsValid())
    {
        return false;
    }

    ADTAGV* IdleAGV = GetIdleAGV();
    if (!IdleAGV)
    {
        return false;
    }

    FVector SpecLocation = Spec->GetActorLocation();

    IdleAGV->SetActorLocation(SpecLocation);
    IdleAGV->AssignSpec(Spec);

    FAGVTask NewTask;
    NewTask.AssignedAGV = IdleAGV;
    NewTask.TargetSpec = Spec;
    NewTask.DestinationTag = DestinationTag;
    NewTask.bIsCompleted = false;

    ActiveTasks.Add(NewTask);

    IdleAGV->MoveToDestination(DestinationTag);

    return true;
}

void UAGVManagerSubsystem::ScanForAGVs()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    for (TActorIterator<ADTAGV> It(World); It; ++It)
    {
        RegisterAGV(*It);
    }
}

void UAGVManagerSubsystem::UpdateActiveTasks()
{
    const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();

    for (int32 i = ActiveTasks.Num() - 1; i >= 0; --i)
    {
        FAGVTask& Task = ActiveTasks[i];

        if (!Task.AssignedAGV || !Task.TargetSpec)
        {
            ActiveTasks.RemoveAt(i);
            continue;
        }

        UAbilitySystemComponent* ASC = Task.AssignedAGV->GetAbilitySystemComponent();
        if (!ASC)
        {
            continue;
        }

        if (!ASC->HasMatchingGameplayTag(GameplayTags.State_AGV_MoveToCell) &&
            !Task.bIsCompleted)
        {
            Task.AssignedAGV->ReleaseSpec();
            Task.bIsCompleted = true;

            OnAGVMovementComplete(Task.AssignedAGV);

            ActiveTasks.RemoveAt(i);
        }
    }
}

void UAGVManagerSubsystem::OnAGVMovementComplete(ADTAGV* AGV)
{
    if (!AGV)
    {
        return;
    }

    if (UAbilitySystemComponent* ASC = AGV->GetAbilitySystemComponent())
    {
        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
        ASC->AddLooseGameplayTag(GameplayTags.State_AGV_Idle);
    }
}