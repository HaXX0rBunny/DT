
#pragma once

#include "CoreMinimal.h"
#include "AttributeSetMacros.h"
#include "AssemblyLineAttributes.generated.h"

UCLASS()
class DT_API UAssemblyLineAttributes : public UAttributeSet
{
    GENERATED_BODY()

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_ProductionProgress)
    FGameplayAttributeData ProductionProgress;
    ATTRIBUTE_ACCESSORS(UAssemblyLineAttributes, ProductionProgress);

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Efficiency)
    FGameplayAttributeData Efficiency;
    ATTRIBUTE_ACCESSORS(UAssemblyLineAttributes, Efficiency);

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_WearAndTear)
    FGameplayAttributeData WearAndTear;
    ATTRIBUTE_ACCESSORS(UAssemblyLineAttributes, WearAndTear);

protected:
    UFUNCTION()
    virtual void OnRep_ProductionProgress(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    virtual void OnRep_Efficiency(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    virtual void OnRep_WearAndTear(const FGameplayAttributeData& OldValue);
};