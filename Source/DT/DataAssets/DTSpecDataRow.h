#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DataAssets/DTSpecData.h"
#include "DTSpecDataRow.generated.h"

USTRUCT(BlueprintType)
struct FDTSpecDataRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UDTSpecData> SpecData;
};