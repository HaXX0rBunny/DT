

#pragma once

#include "CoreMinimal.h"
#include "GASCore/DT_GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GA_Robot_PerformAssembly.generated.h"

class ASpec;
class ADTCollaborativeRobot;

UCLASS()
class DT_API UGA_Robot_PerformAssembly : public UDT_GameplayAbility
{
    GENERATED_BODY()

public:
    UGA_Robot_PerformAssembly();

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Assembly")
    FGameplayTag AssemblyWorkType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Assembly")
    float AssemblyDuration = 5.0f;

private:
    void OnAssemblyComplete(FGameplayEventData Payload);

    FDelegateHandle TaskCompleteHandle;
};