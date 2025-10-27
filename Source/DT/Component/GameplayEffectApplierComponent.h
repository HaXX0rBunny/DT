#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectApplierComponent.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EEffectApplicationTiming : uint8
{
    OnBeginPlay     UMETA(DisplayName = "On Begin Play"),
    OnEndPlay       UMETA(DisplayName = "On End Play"),
    Manual          UMETA(DisplayName = "Manual"),
    OnTagAdded      UMETA(DisplayName = "On Tag Added"),
    OnTagRemoved    UMETA(DisplayName = "On Tag Removed")
};

USTRUCT(BlueprintType)
struct FEffectApplicationData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    TSubclassOf<UGameplayEffect> EffectClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    float EffectLevel = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    EEffectApplicationTiming ApplicationTiming = EEffectApplicationTiming::OnBeginPlay;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect", meta = (EditCondition = "ApplicationTiming == EEffectApplicationTiming::OnTagAdded || ApplicationTiming == EEffectApplicationTiming::OnTagRemoved"))
    FGameplayTag TriggerTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    bool bRemoveOnEndPlay = true;
};

/**
 * GameplayEffect를 자동으로 적용하고 관리하는 컴포넌트
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DT_API UGameplayEffectApplierComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UGameplayEffectApplierComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintCallable, Category = "Effect Applier")
    FActiveGameplayEffectHandle ApplyEffect(TSubclassOf<UGameplayEffect> EffectClass, float Level = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Effect Applier")
    void RemoveEffect(FActiveGameplayEffectHandle EffectHandle);

    UFUNCTION(BlueprintCallable, Category = "Effect Applier")
    void RemoveAllEffects();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Applier")
    TArray<FEffectApplicationData> EffectsToApply;

private:
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> CachedASC;

    TArray<FActiveGameplayEffectHandle> ActiveEffectHandles;
    TMap<FGameplayTag, FDelegateHandle> TagDelegateHandles;

    UAbilitySystemComponent* GetAbilitySystemComponent();
    void ApplyEffectsByTiming(EEffectApplicationTiming Timing);
    void SetupTagBasedEffects();
    void OnTagChanged(const FGameplayTag Tag, int32 NewCount, const FEffectApplicationData& EffectData);
};