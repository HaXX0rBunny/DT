

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h" 
#include "APISynchronizaion/DTSyncActor.h"
#include "DT_DataManager.generated.h"

class AInteractiveActorBase;
class ASpec;
class UDTSpecData;
class UDTCellData;
class ADestinationPoint;
class ADTSyncActor;
class UDataTable;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMonitoredAttributeChanged,const FString&, ActorID, float, NewValue);

UCLASS(Config = Game)
class DT_API UDT_DataManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Digital Twin|Factory")
    ASpec* CreateNewSpec(FGameplayTag SpecTypeTag, const FTransform& SpawnTransform);

    UFUNCTION(BlueprintCallable, Category = "Digital Twin|Factory")
    bool GetDestinationLocation(FGameplayTag DestinationTag, FVector& OutLocation);

    UPROPERTY(BlueprintAssignable, Category = "Digital Twin")
    FOnMonitoredAttributeChanged OnProductionProgressChanged;

    UPROPERTY(BlueprintAssignable, Category = "Digital Twin")
    FOnMonitoredAttributeChanged OnBatteryLevelChanged;
protected:
    // Editor 또는 DefaultEngine.ini에서 설정할 데이터 테이블 경로
    UPROPERTY(Config, EditDefaultsOnly, Category = "Data Management")
    TSoftObjectPtr<UDataTable> SpecDataTablePath;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Data Management")
    TSoftObjectPtr<UDataTable> CellDataTablePath;

private:
    void LoadAllDataAssets();
    void ScanWorldForActors();
    void RegisterActor(AInteractiveActorBase* Actor);

    UFUNCTION()
    void OnApiDataReceived(const TArray<FFactorySyncData>& ReceivedData);

    UFUNCTION()
    void PollFactoryStatus();

    void OnProductionProgressChangedCallback(const FOnAttributeChangeData& Data, FString ActorID);
    void OnBatteryLevelChangedCallback(const FOnAttributeChangeData& Data, FString ActorID);

    UPROPERTY()
    TMap<FGameplayTag, TObjectPtr<UDTSpecData>> SpecDataMap;

    UPROPERTY()
    TMap<FGameplayTag, TObjectPtr<UDTCellData>> CellDataMap;

    UPROPERTY()
    TMap<FGameplayTag, TObjectPtr<ADestinationPoint>> DestinationPoints;

    UPROPERTY()
    TObjectPtr<ADTSyncActor> SyncActor;

    TMap<FString, FDelegateHandle> BoundDelegateHandles;
    FTimerHandle ScanTimerHandle;
    FTimerHandle PollTimerHandle;
};
