#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitySpec.h"
#include "AbilityGranterComponent.generated.h"

class UGameplayAbility;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FAbilityGrantData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    TSubclassOf<UGameplayAbility> AbilityClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    int32 AbilityLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    int32 InputID = -1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    bool bAutoActivate = false;
};

/**
 * Ability를 자동으로 부여하고 관리하는 컴포넌트
 * Actor에 부착하면 BeginPlay 시 자동으로 Ability를 부여합니다
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DT_API UAbilityGranterComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAbilityGranterComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintCallable, Category = "Ability Granter")
    void GrantAbilities();

    UFUNCTION(BlueprintCallable, Category = "Ability Granter")
    void RemoveAbilities();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Granter")
    TArray<FAbilityGrantData> AbilitiesToGrant;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Granter")
    bool bAutoGrantOnBeginPlay = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Granter")
    bool bAutoRemoveOnEndPlay = true;

private:
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> CachedASC;

    TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;

    UAbilitySystemComponent* GetAbilitySystemComponent();
};