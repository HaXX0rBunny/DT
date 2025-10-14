

#include "SmartFactory/DestinationPoint.h"

ADestinationPoint::ADestinationPoint()
{
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;
}

