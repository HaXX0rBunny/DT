#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DataAssets/DTCellData.h"
#include "DTCellDataRow.generated.h"

USTRUCT(BlueprintType)
struct FDTCellDataRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UDTCellData> CellData;
};