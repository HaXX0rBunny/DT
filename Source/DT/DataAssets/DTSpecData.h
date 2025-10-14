#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DTSpecData.generated.h"

UCLASS(BlueprintType)
class DT_API UDTSpecData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spec")
    FGameplayTag SpecTypeTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spec")
    float TotalWorkAmount;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
    TObjectPtr<UStaticMesh> SpecMesh;
};