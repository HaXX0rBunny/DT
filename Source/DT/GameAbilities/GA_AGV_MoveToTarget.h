#pragma once

#include "CoreMinimal.h"
#include "GASCore/DT_GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GA_AGV_MoveToTarget.generated.h"

class ADTAGV;

UCLASS()
class DT_API UGA_AGV_MoveToTarget : public UDT_GameplayAbility
{
    GENERATED_BODY()

public:
    UGA_AGV_MoveToTarget();

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGV")
    FGameplayTag TargetDestinationTag;

private:
    void OnMoveCompleted(const FGameplayEventData* Payload);

    FDelegateHandle MoveCompletedHandle;
};