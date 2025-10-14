

#include "Manager/FactoryLayoutManager.h"
#include "PCGComponent.h"
#include "Components/SceneComponent.h" 
AFactoryLayoutManager::AFactoryLayoutManager()
{
    PrimaryActorTick.bCanEverTick = false;
    USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;
    PCGComponent = CreateDefaultSubobject<UPCGComponent>(TEXT("PCGComponent"));
}

void AFactoryLayoutManager::GenerateFactoryLayout()
{
    if (PCGComponent)
    {
        PCGComponent->Generate();
    }
}