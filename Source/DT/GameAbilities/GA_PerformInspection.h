

#pragma once

#include "CoreMinimal.h"
#include "GASCore/DT_GameplayAbility.h"
#include "GA_PerformInspection.generated.h"

class ASpec;

UCLASS()
class DT_API UGA_PerformInspection : public UDT_GameplayAbility
{
    GENERATED_BODY()

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inspection")
    float DefectRate = 0.05f;

private:
    void PerformInspectionOnSpec(ASpec* Spec);
};