#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DTCellData.generated.h"

UCLASS(BlueprintType)
class DT_API UDTCellData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cell")
    FGameplayTag CellTypeTag;
    // 이 셀에 스폰될 협동 로봇 클래스, 수행할 어빌리티 등 정의
};
