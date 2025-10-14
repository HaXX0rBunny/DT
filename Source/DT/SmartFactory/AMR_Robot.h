

#pragma once

#include "CoreMinimal.h"
#include "Actor/InteractiveActorBase.h"
#include "AMR_Robot.generated.h"
/**
 * 
 */
class UAMR_Attributes;
class UFloatingPawnMovement;

UCLASS()
class DT_API AAMR_Robot : public AInteractiveActorBase
{
    GENERATED_BODY()
public:
    AAMR_Robot();

protected:
    UPROPERTY(Transient)
    TObjectPtr<UAMR_Attributes> AMRAttributes;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    TObjectPtr<UFloatingPawnMovement> MovementComponent;
};
