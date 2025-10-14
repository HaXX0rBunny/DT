

#pragma once

#include "CoreMinimal.h"
#include "AttributeSetMacros.h"
#include "AMR_Attributes.generated.h"
/**
 * 
 */
UCLASS()
class DT_API UAMR_Attributes : public UAttributeSet
{
    GENERATED_BODY()

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_BatteryLevel)
    FGameplayAttributeData BatteryLevel;
    ATTRIBUTE_ACCESSORS(UAMR_Attributes, BatteryLevel);

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxBatteryLevel)
    FGameplayAttributeData MaxBatteryLevel;
    ATTRIBUTE_ACCESSORS(UAMR_Attributes, MaxBatteryLevel);

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CurrentLoad)
    FGameplayAttributeData CurrentLoad;
    ATTRIBUTE_ACCESSORS(UAMR_Attributes, CurrentLoad);

protected:
    UFUNCTION()
    virtual void OnRep_BatteryLevel(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    virtual void OnRep_MaxBatteryLevel(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    virtual void OnRep_CurrentLoad(const FGameplayAttributeData& OldValue);
};