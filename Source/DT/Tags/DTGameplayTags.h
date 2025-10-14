

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FDTGameplayTags
{
public:
    static const FDTGameplayTags& Get() { return GameplayTags; }
    static void InitializeNativeTags();

    // --- 공통 상태 태그 ---
    FGameplayTag State_Busy;
    FGameplayTag State_Idle;
    FGameplayTag State_Error;
    FGameplayTag Event_Task_Complete;

    // --- 로봇 태그 ---
    FGameplayTag State_CellRobot_Idle;
    FGameplayTag State_CellRobot_Working;
    FGameplayTag Type_Manipulator;
    FGameplayTag Type_WelderRobot;
    FGameplayTag Type_AGV;
    FGameplayTag Type_AMR;
    FGameplayTag State_AGV_Idle;
    FGameplayTag State_AGV_WaitCell;
    FGameplayTag State_AGV_MoveToCell;
    FGameplayTag State_AGV_ReturnToBase;
    FGameplayTag State_AGV_AlignBase;
    FGameplayTag Trigger_Ability_AGV_PrepareWork;
    FGameplayTag Trigger_Ability_AGV_AssignSpec;
    FGameplayTag Event_AGV_MoveCompleted;

    // --- Spec 태그 ---
    FGameplayTag State_Spec_Idle;
    FGameplayTag State_Spec_WorkProgress;
    FGameplayTag State_Spec_WorkComplete;
    FGameplayTag Type_Spec_ModelA;
    FGameplayTag Type_Spec_ModelB;
    FGameplayTag Type_Spec_ModelC;
    FGameplayTag Product_Quality_Passed;
    FGameplayTag Product_Quality_Failed;

    // --- 타겟 포인트 태그 ---
    FGameplayTag TargetPoint_AGV_Base_1;
    FGameplayTag TargetPoint_Cell_Enterance;
    FGameplayTag TargetPoint_Cell_Work;

    // --- 셀 태그 ---
    FGameplayTag State_Cell_Idle;
    FGameplayTag State_Cell_Progress;
    FGameplayTag State_Cell_Complete;
    FGameplayTag Type_Cell_Storage;
    FGameplayTag Type_Cell_Assemble_Wheel;
    FGameplayTag Type_Cell_Assemble_Bumper;
    FGameplayTag Type_Cell_Paint;
    FGameplayTag Type_Cell_Inspection;
    FGameplayTag Trigger_Ability_Cell_AssignSpec;

    // --- 어빌리티 태그 ---
    FGameplayTag Ability_AGV_MoveToTarget;
    FGameplayTag Ability_Robot_PerformAssembly;
    FGameplayTag Ability_Cell_PerformInspection;

    // --- 예지 보전 태그 ---
    FGameplayTag Event_Maintenance_Required;

private:
    static FDTGameplayTags GameplayTags;


};