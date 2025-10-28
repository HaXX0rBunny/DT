#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilityTriggerComponent.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;



UENUM(BlueprintType)
enum class EAbilityTriggerType : uint8
{
    OnTagAdded          UMETA(DisplayName = "On Tag Added"),
    OnTagRemoved        UMETA(DisplayName = "On Tag Removed"),
    OnAttributeChanged  UMETA(DisplayName = "On Attribute Changed"),
    OnGameplayEvent     UMETA(DisplayName = "On Gameplay Event"),
    Periodic            UMETA(DisplayName = "Periodic")
};

USTRUCT(BlueprintType)
struct FDTAbilityTriggerData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger")
    TSubclassOf<UGameplayAbility> AbilityToTrigger;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger")
    EAbilityTriggerType TriggerType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger")
    FGameplayTag TriggerTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger", meta = (EditCondition = "TriggerType == EAbilityTriggerType::OnAttributeChanged"))
    FGameplayAttribute TriggerAttribute;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger", meta = (EditCondition = "TriggerType == EAbilityTriggerType::OnAttributeChanged"))
    float AttributeThreshold = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger", meta = (EditCondition = "TriggerType == EAbilityTriggerType::OnAttributeChanged"))
    bool bTriggerWhenAbove = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger", meta = (EditCondition = "TriggerType == EAbilityTriggerType::Periodic"))
    float PeriodicInterval = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger")
    bool bEnabled = true;
};

/**
 * Ability를 자동으로 트리거하는 컴포넌트
 * Tag, Attribute, Event 등의 조건에 따라 Ability 활성화
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DT_API UAbilityTriggerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAbilityTriggerComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintCallable, Category = "Ability Trigger")
    void EnableTrigger(int32 TriggerIndex, bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "Ability Trigger")
    void EnableAllTriggers(bool bEnable);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Trigger")
    TArray<FDTAbilityTriggerData> TriggerSetup;

private:
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> CachedASC;

    TMap<int32, FDelegateHandle> TagDelegateHandles;
    TMap<int32, FDelegateHandle> AttributeDelegateHandles;
    TMap<int32, FDelegateHandle> EventDelegateHandles;
    TMap<int32, FTimerHandle> PeriodicTimerHandles;

    UAbilitySystemComponent* GetAbilitySystemComponent();
    void SetupTriggers();
    void CleanupTriggers();

    void SetupTagTrigger(int32 Index, const FDTAbilityTriggerData& TriggerData);
    void SetupAttributeTrigger(int32 Index, const FDTAbilityTriggerData& TriggerData);
    void SetupEventTrigger(int32 Index, const FDTAbilityTriggerData& TriggerData);
    void SetupPeriodicTrigger(int32 Index, const FDTAbilityTriggerData& TriggerData);

    void OnTagChanged(const FGameplayTag Tag, int32 NewCount, int32 TriggerIndex);
    void OnAttributeChanged(const FOnAttributeChangeData& Data, int32 TriggerIndex);
    void OnGameplayEvent(const FGameplayEventData* Payload, int32 TriggerIndex);

    void OnPeriodicTrigger(int32 TriggerIndex);

    void TriggerAbility(const FDTAbilityTriggerData& TriggerData);
};