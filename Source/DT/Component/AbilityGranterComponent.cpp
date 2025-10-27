#include "AbilityGranterComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbility.h"

UAbilityGranterComponent::UAbilityGranterComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAbilityGranterComponent::BeginPlay()
{
    Super::BeginPlay();

    if (bAutoGrantOnBeginPlay)
    {
        GrantAbilities();
    }
}

void UAbilityGranterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (bAutoRemoveOnEndPlay)
    {
        RemoveAbilities();
    }

    Super::EndPlay(EndPlayReason);
}

void UAbilityGranterComponent::GrantAbilities()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!ASC)
    {
        UE_LOG(LogTemp, Warning, TEXT("AbilityGranterComponent: No ASC found on %s"), *GetOwner()->GetName());
        return;
    }

    for (const FAbilityGrantData& GrantData : AbilitiesToGrant)
    {
        if (!GrantData.AbilityClass)
        {
            continue;
        }

        FGameplayAbilitySpec AbilitySpec(
            GrantData.AbilityClass,
            GrantData.AbilityLevel,
            GrantData.InputID,
            GetOwner()
        );

        FGameplayAbilitySpecHandle SpecHandle = ASC->GiveAbility(AbilitySpec);
        GrantedAbilityHandles.Add(SpecHandle);

        if (GrantData.bAutoActivate)
        {
            ASC->TryActivateAbility(SpecHandle);
        }

        UE_LOG(LogTemp, Log, TEXT("AbilityGranterComponent: Granted %s to %s"),
            *GrantData.AbilityClass->GetName(),
            *GetOwner()->GetName());
    }
}

void UAbilityGranterComponent::RemoveAbilities()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!ASC)
    {
        return;
    }

    for (const FGameplayAbilitySpecHandle& Handle : GrantedAbilityHandles)
    {
        ASC->ClearAbility(Handle);
    }

    GrantedAbilityHandles.Empty();
}

UAbilitySystemComponent* UAbilityGranterComponent::GetAbilitySystemComponent()
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