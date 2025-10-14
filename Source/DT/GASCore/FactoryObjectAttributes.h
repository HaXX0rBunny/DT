

#pragma once

#include "CoreMinimal.h"
#include "AttributeSetMacros.h"
#include "FactoryObjectAttributes.generated.h"


UCLASS()
class DT_API UFactoryObjectAttributes : public UAttributeSet
{
    GENERATED_BODY()

public:
    UFactoryObjectAttributes();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UFactoryObjectAttributes, Health);

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UFactoryObjectAttributes, MaxHealth);

    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_OperationalStatus)
    FGameplayAttributeData OperationalStatus;
    ATTRIBUTE_ACCESSORS(UFactoryObjectAttributes, OperationalStatus);

protected:
    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
    UFUNCTION()
    virtual void OnRep_OperationalStatus(const FGameplayAttributeData& OldOperationalStatus);
};