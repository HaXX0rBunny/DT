#include "GE_BatteryDrain.h"
#include "GASCore/AMR_Attributes.h"

UGE_BatteryDrain::UGE_BatteryDrain()
{
    // Effect 기본 설정
    DurationPolicy = EGameplayEffectDurationType::Infinite;

    // Period 설정 (1초마다 실행)
    Period.Value = 1.0f;
    bExecutePeriodicEffectOnApplication = true;

    // Modifiers 추가 - 배터리 레벨 감소
    FGameplayModifierInfo ModifierInfo;
    ModifierInfo.Attribute = UAMR_Attributes::GetBatteryLevelAttribute();
    ModifierInfo.ModifierOp = EGameplayModOp::Additive;

    // Magnitude 설정 - 초당 0.5% 감소
    FGameplayEffectModifierMagnitude Magnitude;
    FScalableFloat ScalableValue;
    ScalableValue.Value = -0.5f;
    Magnitude.SetScalableFloatMagnitude(ScalableValue);
    ModifierInfo.ModifierMagnitude = Magnitude;

    Modifiers.Add(ModifierInfo);

    // 스택 설정
    StackingType = EGameplayEffectStackingType::None;

    // 태그 설정
    FInheritedTagContainer TagContainer;
    InheritableOwnedTagsContainer = TagContainer;
}