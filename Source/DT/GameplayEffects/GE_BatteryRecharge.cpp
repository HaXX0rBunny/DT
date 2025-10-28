#include "GE_BatteryRecharge.h"
#include "GASCore/AMR_Attributes.h"

UGE_BatteryRecharge::UGE_BatteryRecharge()
{
    // Effect 기본 설정
    DurationPolicy = EGameplayEffectDurationType::Infinite;
    
    // Period 설정 (0.5초마다 실행)
    Period.Value = 0.5f;
    bExecutePeriodicEffectOnApplication = true;

    // Modifiers 추가 - 배터리 레벨 증가
    FGameplayModifierInfo ModifierInfo;
    ModifierInfo.Attribute = UAMR_Attributes::GetBatteryLevelAttribute();
    ModifierInfo.ModifierOp = EGameplayModOp::Additive;
    
    // Magnitude 설정 - 초당 2% 회복
    FScalableFloat ScalableValue(1.0f);
    ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(ScalableValue);

    
    Modifiers.Add(ModifierInfo);

    // 스택 설정
    StackingType = EGameplayEffectStackingType::None;

    // 배터리가 최대치를 넘지 않도록 Clamping
    // 이는 블루프린트 또는 AttributeSet의 PreAttributeChange에서 처리
}