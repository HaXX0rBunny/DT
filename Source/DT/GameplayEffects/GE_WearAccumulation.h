#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_BatteryRecharge.generated.h"

/**
 * AGV 및 AMR 로봇의 배터리 충전 GameplayEffect
 * 충전소에서 주기적으로 배터리를 회복시킵니다
 */
UCLASS()
class DT_API UGE_BatteryRecharge : public UGameplayEffect
{
    GENERATED_BODY()

public:
    UGE_BatteryRecharge();
};