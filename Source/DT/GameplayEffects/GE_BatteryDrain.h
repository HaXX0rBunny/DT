#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_BatteryDrain.generated.h"

/**
 * AGV 및 AMR 로봇의 배터리 소모 GameplayEffect
 * 주기적으로 배터리 레벨을 감소시킵니다
 */
UCLASS()
class DT_API UGE_BatteryDrain : public UGameplayEffect
{
    GENERATED_BODY()

public:
    UGE_BatteryDrain();
};