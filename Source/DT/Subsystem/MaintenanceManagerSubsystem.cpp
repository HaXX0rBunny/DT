

#include "Subsystem/MaintenanceManagerSubsystem.h"
#include "Subsystem/FactoryManagerSubsystem.h"
#include "SmartFactory/FactoryEntityBase.h"
#include "SmartFactory/FactoryAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Tags/DTGameplayTags.h"

void UMaintenanceManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // FactoryManager가 액터를 모두 스폰한 후 기계들을 등록하도록 타이머 설정
}

void UMaintenanceManagerSubsystem::RegisterMachine(AFactoryEntityBase* Machine)
{
    if (Machine && Machine->GetAbilitySystemComponent())
    {
        MonitoredMachines.Add(Machine);
        Machine->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(UFactoryAttributeSet::GetWearLevelAttribute()).AddUObject(this, &UMaintenanceManagerSubsystem::OnWearLevelChanged);
    }
}

void UMaintenanceManagerSubsystem::OnWearLevelChanged(const FOnAttributeChangeData& Data)
{
    // 임계값 (예: 80%) 초과 시 유지보수 이벤트 발생
    if (Data.NewValue >= 0.8f)
    {
        // Data.GEModData->EffectSpec.GetEffectContext()를 통해 어떤 액터인지 알 수 있음
        // 해당 액터에게 GameplayEvent(Event.Maintenance.Required)를 보냄
    }
}
