
#include "SmartFactory/AMR_Robot.h"
#include "GASCore/AMR_Attributes.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

AAMR_Robot::AAMR_Robot()
{
	AMRAttributes = CreateDefaultSubobject<UAMR_Attributes>(TEXT("AMRAttributes"));
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
}
