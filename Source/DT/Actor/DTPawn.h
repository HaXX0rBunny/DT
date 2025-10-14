

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DTPawn.generated.h"

class UFloatingPawnMovement;
UCLASS()
class DT_API ADTPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADTPawn();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFloatingPawnMovement> MovementComponent;
	
	
};
