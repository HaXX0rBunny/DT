
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

void ASpec::BeginPlay()
{
    Super::BeginPlay();

    if (AbilitySystemComponent && SpecAttributes)
    {
        AbilitySystemComponent->AddAttributeSetSubobject(SpecAttributes.Get());
        AbilitySystemComponent->InitStats(USpecAttributes::StaticClass(), nullptr);
        AbilitySystemComponent->SetNumericAttributeBase(USpecAttributes::GetWorkProgressAttribute(), 0.0f);
    }
}

void ASpec::InitializeSpec(UDTSpecData* SpecData)
{
    if (SpecData && AbilitySystemComponent)
    {
        MeshComponent->SetStaticMesh(SpecData->SpecMesh);
        AbilitySystemComponent->AddLooseGameplayTag(SpecData->SpecTypeTag);
    }
}