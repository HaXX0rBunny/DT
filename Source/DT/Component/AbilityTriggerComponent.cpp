#include "AbilityTriggerComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

#include "GASCore/DT_GameplayAbility.h"
#include "Engine/World.h"
#include "TimerManager.h"

UAbilityTriggerComponent::UAbilityTriggerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAbilityTriggerComponent::BeginPlay()
{
    Super::BeginPlay();
    SetupTriggers();
}

void UAbilityTriggerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CleanupTriggers();
    Super::EndPlay(EndPlayReason);
}

void UAbilityTriggerComponent::EnableTrigger(int32 TriggerIndex, bool bEnable)
{
    if (TriggerSetup.IsValidIndex(TriggerIndex))
    {
        TriggerSetup[TriggerIndex].bEnabled = bEnable;
    }
}

void UAbilityTriggerComponent::EnableAllTriggers(bool bEnable)
{
    for (FDTAbilityTriggerData& Trigger : TriggerSetup)
    {
        Trigger.bEnabled = bEnable;
    }
}

UAbilitySystemComponent* UAbilityTriggerComponent::GetAbilitySystemComponent()
{
    if (CachedASC)
    {
        return CachedASC;
    }

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return nullptr;
    }

    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
    {
        CachedASC = ASI->GetAbilitySystemComponent();
        return CachedASC;
    }

    CachedASC = Owner->FindComponentByClass<UAbilitySystemComponent>();
    return CachedASC;
}

void UAbilityTriggerComponent::SetupTriggers()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!ASC)
    {
        UE_LOG(LogTemp, Warning, TEXT("AbilityTriggerComponent: No ASC found on %s"), *GetOwner()->GetName());
        return;
    }

    for (int32 i = 0; i < TriggerSetup.Num(); ++i)
    {
        const FDTAbilityTriggerData& TriggerData = TriggerSetup[i];

        if (!TriggerData.bEnabled || !TriggerData.AbilityToTrigger)
        {
            continue;
        }

        switch (TriggerData.TriggerType)
        {
        case EAbilityTriggerType::OnTagAdded:
        case EAbilityTriggerType::OnTagRemoved:
            SetupTagTrigger(i, TriggerData);
            break;

        case EAbilityTriggerType::OnAttributeChanged:
            SetupAttributeTrigger(i, TriggerData);
            break;

        case EAbilityTriggerType::OnGameplayEvent:
            SetupEventTrigger(i, TriggerData);
            break;

        case EAbilityTriggerType::Periodic:
            SetupPeriodicTrigger(i, TriggerData);
            break;
        }
    }
}

void UAbilityTriggerComponent::CleanupTriggers()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

    if (ASC)
    {
        for (const auto& Pair : TagDelegateHandles)
        {
            const FDTAbilityTriggerData& TriggerData = TriggerSetup[Pair.Key];
            ASC->UnregisterGameplayTagEvent(Pair.Value, TriggerData.TriggerTag);
        }

        for (const auto& Pair : AttributeDelegateHandles)
        {
            const FDTAbilityTriggerData& TriggerData = TriggerSetup[Pair.Key];
            ASC->GetGameplayAttributeValueChangeDelegate(TriggerData.TriggerAttribute).Remove(Pair.Value);
        }

        for (const auto& Pair : EventDelegateHandles)
        {
            const FDTAbilityTriggerData& TriggerData = TriggerSetup[Pair.Key];
            if (TriggerData.TriggerTag.IsValid())
            {
                ASC->GenericGameplayEventCallbacks.FindOrAdd(TriggerData.TriggerTag).Remove(Pair.Value);
            }
        }
    }

    if (UWorld* World = GetWorld())
    {
        for (auto& Pair : PeriodicTimerHandles)
        {
            World->GetTimerManager().ClearTimer(Pair.Value);
        }
    } 

    TagDelegateHandles.Empty();
    AttributeDelegateHandles.Empty();
    EventDelegateHandles.Empty();
    PeriodicTimerHandles.Empty();
}

void UAbilityTriggerComponent::SetupTagTrigger(int32 Index, const FDTAbilityTriggerData& TriggerData)
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!ASC || !TriggerData.TriggerTag.IsValid())
    {
        return;
    }

    FDelegateHandle Handle = ASC->RegisterGameplayTagEvent(TriggerData.TriggerTag)
        .AddUObject(this, &UAbilityTriggerComponent::OnTagChanged, Index);

    TagDelegateHandles.Add(Index, Handle);

    UE_LOG(LogTemp, Log, TEXT("AbilityTriggerComponent: Setup tag trigger for %s"), *TriggerData.TriggerTag.ToString());
}

void UAbilityTriggerComponent::SetupAttributeTrigger(int32 Index, const FDTAbilityTriggerData& TriggerData)
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!ASC || !TriggerData.TriggerAttribute.IsValid())
    {
        return;
    }

    FDelegateHandle Handle = ASC->GetGameplayAttributeValueChangeDelegate(TriggerData.TriggerAttribute)
        .AddUObject(this, &UAbilityTriggerComponent::OnAttributeChanged, Index);

    AttributeDelegateHandles.Add(Index, Handle);

    UE_LOG(LogTemp, Log, TEXT("AbilityTriggerComponent: Setup attribute trigger for %s"),
        *TriggerData.TriggerAttribute.GetName());
}

void UAbilityTriggerComponent::SetupEventTrigger(int32 Index, const FDTAbilityTriggerData& TriggerData)
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!ASC || !TriggerData.TriggerTag.IsValid())
    {
        return;
    }

    FDelegateHandle Handle = ASC->GenericGameplayEventCallbacks.FindOrAdd(TriggerData.TriggerTag)
        .AddUObject(this, &UAbilityTriggerComponent::OnGameplayEvent, Index);

    EventDelegateHandles.Add(Index, Handle);

    UE_LOG(LogTemp, Log, TEXT("AbilityTriggerComponent: Setup event trigger for %s"),
        *TriggerData.TriggerTag.ToString());
}

void UAbilityTriggerComponent::SetupPeriodicTrigger(int32 Index, const FDTAbilityTriggerData& TriggerData)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    FTimerHandle TimerHandle;
    World->GetTimerManager().SetTimer(
        TimerHandle,
        [this, Index]() { OnPeriodicTrigger(Index); },
        TriggerData.PeriodicInterval,
        true
    );

    PeriodicTimerHandles.Add(Index, TimerHandle);

    UE_LOG(LogTemp, Log, TEXT("AbilityTriggerComponent: Setup periodic trigger (%.1fs)"),
        TriggerData.PeriodicInterval);
}

void UAbilityTriggerComponent::OnTagChanged(const FGameplayTag Tag, int32 NewCount, int32 TriggerIndex)
{
    if (!TriggerSetup.IsValidIndex(TriggerIndex) || !TriggerSetup[TriggerIndex].bEnabled)
    {
        return;
    }

    const FDTAbilityTriggerData& TriggerData = TriggerSetup[TriggerIndex];

    const bool bTagAdded = NewCount > 0;
    const bool bTagRemoved = NewCount == 0;

    if ((TriggerData.TriggerType == EAbilityTriggerType::OnTagAdded && bTagAdded) ||
        (TriggerData.TriggerType == EAbilityTriggerType::OnTagRemoved && bTagRemoved))
    {
        TriggerAbility(TriggerData);
    }
}

void UAbilityTriggerComponent::OnAttributeChanged(const FOnAttributeChangeData& Data, int32 TriggerIndex)
{
    if (!TriggerSetup.IsValidIndex(TriggerIndex) || !TriggerSetup[TriggerIndex].bEnabled)
    {
        return;
    }

    const FDTAbilityTriggerData& TriggerData = TriggerSetup[TriggerIndex];

    const bool bConditionMet = TriggerData.bTriggerWhenAbove ?
        (Data.NewValue > TriggerData.AttributeThreshold) :
        (Data.NewValue < TriggerData.AttributeThreshold);

    if (bConditionMet)
    {
        TriggerAbility(TriggerData);
    }
}

void UAbilityTriggerComponent::OnGameplayEvent(const FGameplayEventData* Payload, int32 TriggerIndex)
{
    if (!TriggerSetup.IsValidIndex(TriggerIndex) || !TriggerSetup[TriggerIndex].bEnabled)
    {
        return;
    }

    TriggerAbility(TriggerSetup[TriggerIndex]);
}

void UAbilityTriggerComponent::OnPeriodicTrigger(int32 TriggerIndex)
{
    if (!TriggerSetup.IsValidIndex(TriggerIndex) || !TriggerSetup[TriggerIndex].bEnabled)
    {
        return;
    }

    TriggerAbility(TriggerSetup[TriggerIndex]);
}

void UAbilityTriggerComponent::TriggerAbility(const FDTAbilityTriggerData& TriggerData)
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!ASC || !TriggerData.AbilityToTrigger)
    {
        return;
    }

    FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(TriggerData.AbilityToTrigger);
    if (Spec)
    {
        ASC->TryActivateAbility(Spec->Handle);

        UE_LOG(LogTemp, Log, TEXT("AbilityTriggerComponent: Triggered %s"),
            *TriggerData.AbilityToTrigger->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AbilityTriggerComponent: Ability %s not found in ASC"),
            *TriggerData.AbilityToTrigger->GetName());
    }
}