

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayEffectTypes.h"
#include "MaintenanceManagerSubsystem.generated.h"

class AFactoryEntityBase;

UCLASS()
class DT_API UMaintenanceManagerSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    void RegisterMachine(AFactoryEntityBase* Machine);

private:
    void OnWearLevelChanged(const FOnAttributeChangeData& Data);

    // 모니터링할 기계 목록
    UPROPERTY()
    TArray<TObjectPtr<AFactoryEntityBase>> MonitoredMachines;
};