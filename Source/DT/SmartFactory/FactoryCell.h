#pragma once

#pragma once

#include "CoreMinimal.h"
#include "SmartFactory/FactoryEntityBase.h"  
#include "GameplayTagContainer.h"
#include "FactoryCell.generated.h"

class UAssemblyLineAttributes;
class UDTCellData;
class ASpec;
class ADTCollaborativeRobot;

/**
 * FactoryCell - 작업 셀
 * 현대자동차 HMGICS의 셀 기반 생산 시스템을 구현
 */
UCLASS()
class DT_API AFactoryCell : public AFactoryEntityBase  
{
    GENERATED_BODY()

public:
    AFactoryCell();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Factory Cell")
    void InitializeCell(UDTCellData* CellData);

    UFUNCTION(BlueprintCallable, Category = "Factory Cell")
    void AssignSpec(ASpec* NewSpec);

    UFUNCTION(BlueprintPure, Category = "Factory Cell")
    ASpec* GetCurrentSpec() const { return CurrentSpec; }

    UFUNCTION(BlueprintPure, Category = "Factory Cell")
    bool IsIdle() const;

    UFUNCTION(BlueprintPure, Category = "Factory Cell")
    float GetProductionProgress() const;

    UFUNCTION(BlueprintPure, Category = "Factory Cell")
    float GetEfficiency() const;

    UFUNCTION(BlueprintCallable, Category = "Factory Cell")
    void RegisterRobot(ADTCollaborativeRobot* Robot);

    UFUNCTION(BlueprintCallable, Category = "Factory Cell")
    void ResetCell();

protected:
    UPROPERTY(Transient)
    TObjectPtr<UAssemblyLineAttributes> AssemblyAttributes;

    UPROPERTY(BlueprintReadOnly, Category = "Factory Cell")
    TObjectPtr<ASpec> CurrentSpec;

    UPROPERTY(BlueprintReadOnly, Category = "Factory Cell")
    TArray<TObjectPtr<ADTCollaborativeRobot>> AssignedRobots;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Factory Cell")
    FGameplayTag CellTypeTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Factory Cell")
    FVector SpecWorkLocation = FVector(0, 0, 100);

    virtual void OnWorkComplete();
    virtual void OnWorkStart();
};