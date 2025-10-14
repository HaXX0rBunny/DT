
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "InteractiveActorBase.generated.h"

class UAbilitySystemComponent;
class UFactoryObjectAttributes;

UCLASS(Abstract)
class DT_API AInteractiveActorBase : public AActor, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AInteractiveActorBase();
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(Transient)
    TObjectPtr<UFactoryObjectAttributes> BaseAttributes;
};