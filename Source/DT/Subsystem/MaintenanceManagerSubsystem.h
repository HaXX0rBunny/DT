

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "MaintenanceManagerSubsystem.generated.h"

class AFactoryEntityBase;

USTRUCT(BlueprintType)
struct FMaintenanceRecord
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TWeakObjectPtr<AFactoryEntityBase> Machine;

    UPROPERTY(BlueprintReadOnly)
    float WearLevel = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float CycleCount = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    FDateTime LastMaintenanceTime;

    UPROPERTY(BlueprintReadOnly)
    bool bRequiresMaintenance = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaintenanceRequired, AFactoryEntityBase*, Machine, float, WearLevel);

UCLASS()
class DT_API UMaintenanceManagerSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Maintenance")
    void RegisterMachine(AFactoryEntityBase* Machine);

    UFUNCTION(BlueprintCallable, Category = "Maintenance")
    void UnregisterMachine(AFactoryEntityBase* Machine);

    UFUNCTION(BlueprintCallable, Category = "Maintenance")
    void PerformMaintenance(AFactoryEntityBase* Machine);

    UFUNCTION(BlueprintCallable, Category = "Maintenance")
    TArray<FMaintenanceRecord> GetMaintenanceRecords() const { return MaintenanceRecords; }

    UPROPERTY(BlueprintAssignable, Category = "Maintenance")
    FOnMaintenanceRequired OnMaintenanceRequired;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Maintenance")
    float WearLevelThreshold = 0.8f;

    UPROPERTY(EditDefaultsOnly, Category = "Maintenance")
    float CycleCountThreshold = 1000.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Maintenance")
    float MaintenanceCheckInterval = 10.0f;

private:
    void ScanForMachines();
    void CheckMaintenanceStatus();
    void OnWearLevelChanged(const FOnAttributeChangeData& Data);
    void OnCycleCountChanged(const FOnAttributeChangeData& Data);
    void UpdateMaintenanceRecord(AFactoryEntityBase* Machine);

    UPROPERTY()
    TArray<TObjectPtr<AFactoryEntityBase>> MonitoredMachines;

    UPROPERTY()
    TArray<FMaintenanceRecord> MaintenanceRecords;

    TMap<TWeakObjectPtr<AFactoryEntityBase>, FDelegateHandle> WearLevelDelegates;
    TMap<TWeakObjectPtr<AFactoryEntityBase>, FDelegateHandle> CycleCountDelegates;

    FTimerHandle MaintenanceCheckTimerHandle;
};