#include "GameplayEffectApplierComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UGameplayEffectApplierComponent::UGameplayEffectApplierComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UGameplayEffectApplierComponent::BeginPlay()
{
    Super::BeginPlay();

    ApplyEffectsByTiming(EEffectApplicationTiming::OnBeginPlay);
    SetupTagBasedEffects();
}

void UGameplayEffectApplierComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ApplyEffectsByTiming(EEffectApplicationTiming::OnEndPlay);
    RemoveAllEffects();

    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (ASC)
    {
        for (const auto& Pair : TagDelegateHandles)
        {
            ASC->UnregisterGameplayTagEvent(Pair.Value, Pair.Key);
        }
    }

    TagDelegateHandles.Empty();

    Super::EndPlay(EndPlayReason);
}

FActiveGameplayEffectHandle UGameplayEffectApplierComponent::ApplyEffect(TSubclassOf<UGameplayEffect> EffectClass, float Level)
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!ASC || !EffectClass)
    {
        return FActiveGameplayEffectHandle();
    }

    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    EffectContext.AddSourceObject(GetOwner());

    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, Level, EffectContext);

    if (SpecHandle.IsValid())
    {
        FActiveGameplayEffectHandle ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        ActiveEffectHandles.Add(ActiveHandle);

        UE_LOG(LogTemp, Log, TEXT("GameplayEffectApplierComponent: Applied %s to %s"),
            *EffectClass->GetName(),
            *GetOwner()->GetName());

        return ActiveHandle;
    }

    return FActiveGameplayEffectHandle();
}

void UGameplayEffectApplierComponent::RemoveEffect(FActiveGameplayEffectHandle EffectHandle)
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!ASC || !EffectHandle.IsValid())
    {
        return;
    }

    ASC->RemoveActiveGameplayEffect(EffectHandle);
    ActiveEffectHandles.Remove(EffectHandle);
}

void UGameplayEffectApplierComponent::RemoveAllEffects()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!ASC)
    {
        return;
    }

    for (const FActiveGameplayEffectHandle& Handle : ActiveEffectHandles)
    {
        if (Handle.IsValid())
        {
            ASC->RemoveActiveGameplayEffect(Handle);
        }
    }

    ActiveEffectHandles.Empty();
}

UAbilitySystemComponent* UGameplayEffectApplierComponent::GetAbilitySystemComponent()
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

void UGameplayEffectApplierComponent::ApplyEffectsByTiming(EEffectApplicationTiming Timing)
{
    for (const FEffectApplicationData& EffectData : EffectsToApply)
    {
        if (EffectData.ApplicationTiming == Timing && EffectData.EffectClass)
        {
            ApplyEffect(EffectData.EffectClass, EffectData.EffectLevel);
        }
    }
}

void UGameplayEffectApplierComponent::SetupTagBasedEffects()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!ASC)
    {
        return;
    }

    for (const FEffectApplicationData& EffectData : EffectsToApply)
    {
        if (EffectData.ApplicationTiming == EEffectApplicationTiming::OnTagAdded ||
            EffectData.ApplicationTiming == EEffectApplicationTiming::OnTagRemoved)
        {
            if (EffectData.TriggerTag.IsValid())
            {
                FDelegateHandle Handle = ASC->RegisterGameplayTagEvent(EffectData.TriggerTag)
                    .AddUObject(this, &UGameplayEffectApplierComponent::OnTagChanged, EffectData);

                TagDelegateHandles.Add(EffectData.TriggerTag, Handle);
            }
        }
    }
}

void UGameplayEffectApplierComponent::OnTagChanged(const FGameplayTag Tag, int32 NewCount, const FEffectApplicationData& EffectData)
{
    const bool bTagAdded = NewCount > 0;
    const bool bTagRemoved = NewCount == 0;

    if ((bTagAdded && EffectData.ApplicationTiming == EEffectApplicationTiming::OnTagAdded) ||
        (bTagRemoved && EffectData.ApplicationTiming == EEffectApplicationTiming::OnTagRemoved))
    {
        ApplyEffect(EffectData.EffectClass, EffectData.EffectLevel);
    }
}