

#include "Subsystem/MaintenanceManagerSubsystem.h"
#include "SmartFactory/FactoryEntityBase.h"
#include "SmartFactory/FactoryAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "Tags/DTGameplayTags.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UMaintenanceManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimerForNextTick(this, &UMaintenanceManagerSubsystem::ScanForMachines);

        World->GetTimerManager().SetTimer(
            MaintenanceCheckTimerHandle,
            this,
            &UMaintenanceManagerSubsystem::CheckMaintenanceStatus,
            MaintenanceCheckInterval,
            true
        );
    }
}

void UMaintenanceManagerSubsystem::Deinitialize()
{
    if (UWorld* World = GetWorld())
        World->GetTimerManager().ClearTimer(MaintenanceCheckTimerHandle);

    for (auto& Pair : WearLevelDelegates)
    {
        if (AFactoryEntityBase* Machine = Pair.Key.Get())
        {
            if (UAbilitySystemComponent* ASC = Machine->GetAbilitySystemComponent())
            {
                ASC->GetGameplayAttributeValueChangeDelegate(
                    UFactoryAttributeSet::GetWearLevelAttribute()).Remove(Pair.Value);
            }
        }
    }

    for (auto& Pair : CycleCountDelegates)
    {
        if (AFactoryEntityBase* Machine = Pair.Key.Get())
        {
            if (UAbilitySystemComponent* ASC = Machine->GetAbilitySystemComponent())
            {
                ASC->GetGameplayAttributeValueChangeDelegate(
                    UFactoryAttributeSet::GetCycleCountAttribute()).Remove(Pair.Value);
            }
        }
    }

    WearLevelDelegates.Empty();
    CycleCountDelegates.Empty();
    MonitoredMachines.Empty();
    MaintenanceRecords.Empty();

    Super::Deinitialize();
}

void UMaintenanceManagerSubsystem::RegisterMachine(AFactoryEntityBase* Machine)
{
    if (!Machine || MonitoredMachines.Contains(Machine))
        return;

    UAbilitySystemComponent* ASC = Machine->GetAbilitySystemComponent();
    if (!ASC)
        return;

    MonitoredMachines.Add(Machine);

    FDelegateHandle WearHandle = ASC->GetGameplayAttributeValueChangeDelegate(
        UFactoryAttributeSet::GetWearLevelAttribute())
        .AddUObject(this, &UMaintenanceManagerSubsystem::OnWearLevelChanged);

    FDelegateHandle CycleHandle = ASC->GetGameplayAttributeValueChangeDelegate(
        UFactoryAttributeSet::GetCycleCountAttribute())
        .AddUObject(this, &UMaintenanceManagerSubsystem::OnCycleCountChanged);

    WearLevelDelegates.Add(Machine, WearHandle);
    CycleCountDelegates.Add(Machine, CycleHandle);

    FMaintenanceRecord NewRecord;
    NewRecord.Machine = Machine;
    NewRecord.LastMaintenanceTime = FDateTime::Now();
    MaintenanceRecords.Add(NewRecord);
}

void UMaintenanceManagerSubsystem::UnregisterMachine(AFactoryEntityBase* Machine)
{
    if (!Machine)
        return;

    MonitoredMachines.Remove(Machine);

    if (FDelegateHandle* WearHandle = WearLevelDelegates.Find(Machine))
    {
        if (UAbilitySystemComponent* ASC = Machine->GetAbilitySystemComponent())
        {
            ASC->GetGameplayAttributeValueChangeDelegate(
                UFactoryAttributeSet::GetWearLevelAttribute()).Remove(*WearHandle);
        }
        WearLevelDelegates.Remove(Machine);
    }

    if (FDelegateHandle* CycleHandle = CycleCountDelegates.Find(Machine))
    {
        if (UAbilitySystemComponent* ASC = Machine->GetAbilitySystemComponent())
        {
            ASC->GetGameplayAttributeValueChangeDelegate(
                UFactoryAttributeSet::GetCycleCountAttribute()).Remove(*CycleHandle);
        }
        CycleCountDelegates.Remove(Machine);
    }

    MaintenanceRecords.RemoveAll([Machine](const FMaintenanceRecord& Record){
            return Record.Machine == Machine;
        });
}

void UMaintenanceManagerSubsystem::PerformMaintenance(AFactoryEntityBase* Machine)
{
    if (!Machine)
        return;

    UAbilitySystemComponent* ASC = Machine->GetAbilitySystemComponent();
    if (!ASC)
        return;

    ASC->SetNumericAttributeBase(UFactoryAttributeSet::GetWearLevelAttribute(), 0.0f);
    ASC->SetNumericAttributeBase(UFactoryAttributeSet::GetCycleCountAttribute(), 0.0f);

    const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
    ASC->RemoveLooseGameplayTag(GameplayTags.Event_Maintenance_Required);

    for (FMaintenanceRecord& Record : MaintenanceRecords)
    {
        if (Record.Machine == Machine)
        {
            Record.WearLevel = 0.0f;
            Record.CycleCount = 0.0f;
            Record.LastMaintenanceTime = FDateTime::Now();
            Record.bRequiresMaintenance = false;
            break;
        }
    }
}

void UMaintenanceManagerSubsystem::ScanForMachines()
{
    UWorld* World = GetWorld();
    if (!World)
        return;

    for (TActorIterator<AFactoryEntityBase> It(World); It; ++It)
        RegisterMachine(*It);
}

void UMaintenanceManagerSubsystem::CheckMaintenanceStatus()
{
    for (AFactoryEntityBase* Machine : MonitoredMachines)
    {
        if (!Machine)
            continue;

        UpdateMaintenanceRecord(Machine);
    }
}

void UMaintenanceManagerSubsystem::OnWearLevelChanged(const FOnAttributeChangeData& Data)
{	
    // 임계값 초과 시 유지보수 이벤트 발생
        // Data.GEModData->EffectSpec.GetEffectContext()를 통해 어떤 액터인지 알 수 있음
        // 해당 액터에게 GameplayEvent(Event.Maintenance.Required)를 보냄
    if (Data.NewValue >= WearLevelThreshold)
    {
        AActor* OwnerActor = Data.GEModData ? Data.GEModData->Target.GetOwnerActor() : nullptr;
        AFactoryEntityBase* Machine = Cast<AFactoryEntityBase>(OwnerActor);

        if (Machine)
        {
            UAbilitySystemComponent* ASC = Machine->GetAbilitySystemComponent();
            if (ASC)
            {
                const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
                ASC->AddLooseGameplayTag(GameplayTags.Event_Maintenance_Required);

                FGameplayEventData EventData;
                EventData.EventMagnitude = Data.NewValue;
                ASC->HandleGameplayEvent(GameplayTags.Event_Maintenance_Required, &EventData);
            }

            OnMaintenanceRequired.Broadcast(Machine, Data.NewValue);

            for (FMaintenanceRecord& Record : MaintenanceRecords)
            {
                if (Record.Machine == Machine)
                {
                    Record.bRequiresMaintenance = true;
                    break;
                }
            }
        }
    }
}

void UMaintenanceManagerSubsystem::OnCycleCountChanged(const FOnAttributeChangeData& Data)
{
    if (Data.NewValue >= CycleCountThreshold)
    {
        AActor* OwnerActor = Data.GEModData ? Data.GEModData->Target.GetOwnerActor() : nullptr;
        AFactoryEntityBase* Machine = Cast<AFactoryEntityBase>(OwnerActor);

        if (Machine)
            UpdateMaintenanceRecord(Machine);
    }
}

void UMaintenanceManagerSubsystem::UpdateMaintenanceRecord(AFactoryEntityBase* Machine)
{
    if (!Machine)
        return;

    UAbilitySystemComponent* ASC = Machine->GetAbilitySystemComponent();
    if (!ASC)
        return;

    const float WearLevel = ASC->GetNumericAttribute(UFactoryAttributeSet::GetWearLevelAttribute());
    const float CycleCount = ASC->GetNumericAttribute(UFactoryAttributeSet::GetCycleCountAttribute());

    for (FMaintenanceRecord& Record : MaintenanceRecords)
    {
        if (Record.Machine == Machine)
        {
            Record.WearLevel = WearLevel;
            Record.CycleCount = CycleCount;
            Record.bRequiresMaintenance = (WearLevel >= WearLevelThreshold) ||
                (CycleCount >= CycleCountThreshold);
            break;
        }
    }
}