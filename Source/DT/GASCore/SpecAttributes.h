

#pragma once

#include "CoreMinimal.h"
#include "AttributeSetMacros.h"
#include "SpecAttributes.generated.h"

UCLASS()
class DT_API USpecAttributes : public UAttributeSet
{
    GENERATED_BODY()

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(BlueprintReadOnly, Category = "Spec", ReplicatedUsing = OnRep_WorkProgress)
    FGameplayAttributeData WorkProgress;
    ATTRIBUTE_ACCESSORS(USpecAttributes, WorkProgress);

protected:
    UFUNCTION()
    virtual void OnRep_WorkProgress(const FGameplayAttributeData& OldValue);
};