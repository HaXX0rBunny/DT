


#include "DTGameModeBase.h"
#include "DTPlayerController.h"
#include "Actor/DTPawn.h"

ADTGameModeBase::ADTGameModeBase()
{
    PlayerControllerClass = ADTPlayerController::StaticClass();
    DefaultPawnClass = ADTPawn::StaticClass();
}



