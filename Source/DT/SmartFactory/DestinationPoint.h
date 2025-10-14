

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "DestinationPoint.generated.h"

UCLASS()
class DT_API ADestinationPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestinationPoint();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Destination")
	FGameplayTag DestinationTag;

};
