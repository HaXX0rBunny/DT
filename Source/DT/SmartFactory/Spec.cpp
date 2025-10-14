
#include "Spec.h"
#include "GASCore/SpecAttributes.h"
#include "DataAssets/DTSpecData.h"
#include "AbilitySystemComponent.h"
#include "Components/StaticMeshComponent.h"

ASpec::ASpec()
{
    SpecAttributes = CreateDefaultSubobject<USpecAttributes>(TEXT("SpecAttributes"));

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;
}

void ASpec::InitializeSpec(UDTSpecData* SpecData)
{
    if (SpecData && AbilitySystemComponent)
    {
        MeshComponent->SetStaticMesh(SpecData->SpecMesh);
        AbilitySystemComponent->AddLooseGameplayTag(SpecData->SpecTypeTag);
    }
}