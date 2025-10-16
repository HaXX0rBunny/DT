#pragma once

#include "CoreMinimal.h"
#include "GASCore/DT_GameplayAbility.h"
#include "GA_Cell_ProcessSpec.generated.h"

class ASpec;
class AFactoryCell;

UCLASS()
class DT_API UGA_Cell_ProcessSpec : public UDT_GameplayAbility
{
    GENERATED_BODY()

public:
    UGA_Cell_ProcessSpec();

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cell")
    float ProcessDuration = 10.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cell")
    float ProgressIncrement = 100.0f;

private:
    void ProcessComplete();
    void UpdateCellState(AFactoryCell* Cell, bool bIsProcessing);

    UPROPERTY()
    TObjectPtr<ASpec> CurrentSpec;

    FTimerHandle ProcessTimerHandle;
};