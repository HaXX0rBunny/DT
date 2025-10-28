#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_WearAccumulation.generated.h"

/**
 * 설비의 마모도 누적 GameplayEffect
 * 작업 수행 중 주기적으로 마모도를 증가시킵니다
 */
UCLASS()
class DT_API UGE_WearAccumulation : public UGameplayEffect
{
    GENERATED_BODY()

public:
    UGE_WearAccumulation();
};