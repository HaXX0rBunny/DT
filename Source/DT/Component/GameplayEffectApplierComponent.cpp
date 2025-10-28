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
    RemoveTagDelegates();
    //맵 초기화
    TagToEffectMap.Empty();

    for (const FEffectApplicationData& EffectData : EffectsToApply)
    {
        if (EffectData.ApplicationTiming == EEffectApplicationTiming::OnTagAdded ||
            EffectData.ApplicationTiming == EEffectApplicationTiming::OnTagRemoved)
        {
            if (EffectData.TriggerTag.IsValid())
            {
                // TriggerTag를 키로 EffectData를 맵에 저장합니다.
                TagToEffectMap.Add(EffectData.TriggerTag, EffectData);

                // addUObject를 사용해 새 콜백 함수를 바인딩합니다.
                //    이제 시그니처가 일치하므로 E0304 오류가 발생하지 않습니다.
                ASC->RegisterGameplayTagEvent(EffectData.TriggerTag)
                    .AddUObject(this, &UGameplayEffectApplierComponent::OnTagChanged);
            }
        }
    }
}

void UGameplayEffectApplierComponent::RemoveTagDelegates()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

    if (ASC)
    {
 
        for (auto& Pair : TagDelegateHandles)
        {
            if (Pair.Key.IsValid() && Pair.Value.IsValid())
            {
                ASC->RegisterGameplayTagEvent(Pair.Key).Remove(Pair.Value);
            }
        }
    }



    TagDelegateHandles.Empty();
    TagToEffectMap.Empty();
}


void UGameplayEffectApplierComponent::OnTagChanged(const FGameplayTag Tag, int32 NewCount)
{

    const FEffectApplicationData* EffectData = TagToEffectMap.Find(Tag);


    if (!EffectData)
    {
        return;
    }

    const bool bTagAdded = NewCount > 0;
    const bool bTagRemoved = NewCount == 0;

    if ((bTagAdded && EffectData->ApplicationTiming == EEffectApplicationTiming::OnTagAdded) ||
        (bTagRemoved && EffectData->ApplicationTiming == EEffectApplicationTiming::OnTagRemoved))
    {
        ApplyEffect(EffectData->EffectClass, EffectData->EffectLevel);
    }
}