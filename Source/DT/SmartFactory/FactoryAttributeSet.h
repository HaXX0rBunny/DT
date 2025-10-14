#pragma once

#include "CoreMinimal.h"
#include "AttributeSetMacros.h"
#include "FactoryAttributeSet.generated.h"

UCLASS()
class DT_API UFactoryAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(BlueprintReadOnly, Category = "Maintenance", ReplicatedUsing = OnRep_WearLevel)
    FGameplayAttributeData WearLevel; // 마모도 (0-1)
    ATTRIBUTE_ACCESSORS(UFactoryAttributeSet, WearLevel);

    UPROPERTY(BlueprintReadOnly, Category = "Maintenance", ReplicatedUsing = OnRep_CycleCount)
    FGameplayAttributeData CycleCount; // 작업 사이클 횟수
    ATTRIBUTE_ACCESSORS(UFactoryAttributeSet, CycleCount);

protected:
    UFUNCTION()
    virtual void OnRep_WearLevel(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    virtual void OnRep_CycleCount(const FGameplayAttributeData& OldValue);
};
