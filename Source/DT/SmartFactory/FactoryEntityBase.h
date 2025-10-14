

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "FactoryEntityBase.generated.h"

class UAbilitySystemComponent;
class UFactoryAttributeSet;
UCLASS(Abstract)
class DT_API AFactoryEntityBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFactoryEntityBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// 예지 보전을 위한 공통 속성 세트
	UPROPERTY(Transient)
	TObjectPtr<UFactoryAttributeSet> FactoryAttributes;

	
	
};
