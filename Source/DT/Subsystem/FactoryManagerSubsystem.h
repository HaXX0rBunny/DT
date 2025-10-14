

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "FactoryManagerSubsystem.generated.h"

class UDataTable;
class AFactoryEntityBase;
class AFactoryCell;
class ADTAGV;

USTRUCT(BlueprintType)
struct FProductionOrder
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FGameplayTag ProductSpecTag;

    UPROPERTY(BlueprintReadOnly)
    int32 Quantity = 1;

    UPROPERTY(BlueprintReadOnly)
    float Priority = 1.0f;
};

UCLASS()
class DT_API UFactoryManagerSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Factory Manager")
    void QueueProductionOrder(const FGameplayTag& ProductSpecTag, int32 Quantity = 1, float Priority = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Factory Manager")
    AFactoryEntityBase* FindAssetByTag(const FGameplayTag& AssetTag) const;

    UFUNCTION(BlueprintCallable, Category = "Factory Manager")
    void StartProduction();

    UFUNCTION(BlueprintCallable, Category = "Factory Manager")
    void StopProduction();
private:
    void PopulateFactory();
    void ProcessProductionQueue();
    void AssignSpecToCell(const FGameplayTag& SpecTag);
    AFactoryCell* FindAvailableCall(const FGameplayTag& CellTypeTag);

    UPROPERTY()
    TMap<FGameplayTag, TWeakObjectPtr<AFactoryEntityBase>> AssetRegistry;
    UPROPERTY()
    TArray<TObjectPtr<AFactoryCell>> AvailableCells;

    UPROPERTY()
    TArray<TObjectPtr<ADTAGV>> AvailableAGVs;

    TQueue<FProductionOrder> ProductionOrderQueue;
    FTimerHandle ProductionTimerHandle;
    bool bIsProducing = false;
};
