#include "GE_WearAccumulation.h"
#include "SmartFactory/FactoryAttributeSet.h"

UGE_WearAccumulation::UGE_WearAccumulation()
{

    DurationPolicy = EGameplayEffectDurationType::Infinite;

 
    Period.Value = 5.0f;
    bExecutePeriodicEffectOnApplication = true;

    {
        FGameplayModifierInfo ModifierInfo;
        ModifierInfo.Attribute = UFactoryAttributeSet::GetWearLevelAttribute();
        ModifierInfo.ModifierOp = EGameplayModOp::Additive;

        FGameplayEffectModifierMagnitude Magnitude;
   
        FScalableFloat ScalableValue(0.01f); // 5초당 1% 증가
        ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(ScalableValue);


        Modifiers.Add(ModifierInfo);
    }

    {
        FGameplayModifierInfo ModifierInfo;
        ModifierInfo.Attribute = UFactoryAttributeSet::GetCycleCountAttribute();
        ModifierInfo.ModifierOp = EGameplayModOp::Additive;


        FScalableFloat ScalableValue(1.0f); // 5초당 1 증가
        ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(ScalableValue);


        Modifiers.Add(ModifierInfo);
    }


    StackingType = EGameplayEffectStackingType::None;
}