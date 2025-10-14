#include "DTGameplayTags.h"
#include "GameplayTagsManager.h"

FDTGameplayTags FDTGameplayTags::GameplayTags;

void FDTGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();

	// --- 공통 상태 태그 ---
	GameplayTags.State_Busy = TagsManager.AddNativeGameplayTag(TEXT("State.Busy"));
	GameplayTags.State_Idle = TagsManager.AddNativeGameplayTag(TEXT("State.Idle"));
	GameplayTags.State_Error = TagsManager.AddNativeGameplayTag(TEXT("State.Error"));
	GameplayTags.Event_Task_Complete = TagsManager.AddNativeGameplayTag(TEXT("Event.Task.Complete"));

	// --- 로봇 태그 ---
	GameplayTags.State_CellRobot_Idle = TagsManager.AddNativeGameplayTag(TEXT("State.CellRobot.Idle"));
	GameplayTags.State_CellRobot_Working = TagsManager.AddNativeGameplayTag(TEXT("State.CellRobot.Working"));
	GameplayTags.Type_Manipulator = TagsManager.AddNativeGameplayTag(TEXT("Type.Manipulator"));
	GameplayTags.Type_WelderRobot = TagsManager.AddNativeGameplayTag(TEXT("Type.WelderRobot"));
	GameplayTags.Type_AGV = TagsManager.AddNativeGameplayTag(TEXT("Type.AGV"));
	GameplayTags.Type_AMR = TagsManager.AddNativeGameplayTag(TEXT("Type.AMR"));
	GameplayTags.State_AGV_Idle = TagsManager.AddNativeGameplayTag(TEXT("State.AGV.Idle"));
	GameplayTags.State_AGV_WaitCell = TagsManager.AddNativeGameplayTag(TEXT("State.AGV.WaitCell"));
	GameplayTags.State_AGV_MoveToCell = TagsManager.AddNativeGameplayTag(TEXT("State.AGV.MoveToCell"));
	GameplayTags.State_AGV_ReturnToBase = TagsManager.AddNativeGameplayTag(TEXT("State.AGV.ReturnToBase"));
	GameplayTags.State_AGV_AlignBase = TagsManager.AddNativeGameplayTag(TEXT("State.AGV.AlignBase"));
	GameplayTags.Trigger_Ability_AGV_PrepareWork = TagsManager.AddNativeGameplayTag(TEXT("Trigger.Ability.AGV.PrepareWork"));
	GameplayTags.Trigger_Ability_AGV_AssignSpec = TagsManager.AddNativeGameplayTag(TEXT("Trigger.Ability.AGV.AssignSpec"));
	GameplayTags.Event_AGV_MoveCompleted = TagsManager.AddNativeGameplayTag(TEXT("Event.AGV.MoveCompleted"));

	// --- Spec 태그 ---
	GameplayTags.State_Spec_Idle = TagsManager.AddNativeGameplayTag(TEXT("State.Spec.Idle"));
	GameplayTags.State_Spec_WorkProgress = TagsManager.AddNativeGameplayTag(TEXT("State.Spec.WorkProgress"));
	GameplayTags.State_Spec_WorkComplete = TagsManager.AddNativeGameplayTag(TEXT("State.Spec.WorkComplete"));
	GameplayTags.Type_Spec_ModelA = TagsManager.AddNativeGameplayTag(TEXT("Type.Spec.ModelA"));
	GameplayTags.Type_Spec_ModelB = TagsManager.AddNativeGameplayTag(TEXT("Type.Spec.ModelB"));
	GameplayTags.Type_Spec_ModelC = TagsManager.AddNativeGameplayTag(TEXT("Type.Spec.ModelC"));
	GameplayTags.Product_Quality_Passed = TagsManager.AddNativeGameplayTag(TEXT("Product.Quality.Passed"));
	GameplayTags.Product_Quality_Failed = TagsManager.AddNativeGameplayTag(TEXT("Product.Quality.Failed"));

	// --- 타겟 포인트 태그 ---
	GameplayTags.TargetPoint_AGV_Base_1 = TagsManager.AddNativeGameplayTag(TEXT("TargetPoint.AGV.Base.1"));
	GameplayTags.TargetPoint_Cell_Enterance = TagsManager.AddNativeGameplayTag(TEXT("TargetPoint.Cell.Enterance"));
	GameplayTags.TargetPoint_Cell_Work = TagsManager.AddNativeGameplayTag(TEXT("TargetPoint.Cell.Work"));

	// --- 셀 태그 ---
	GameplayTags.State_Cell_Idle = TagsManager.AddNativeGameplayTag(TEXT("State.Cell.Idle"));
	GameplayTags.State_Cell_Progress = TagsManager.AddNativeGameplayTag(TEXT("State.Cell.Progress"));
	GameplayTags.State_Cell_Complete = TagsManager.AddNativeGameplayTag(TEXT("State.Cell.Complete"));
	GameplayTags.Type_Cell_Storage = TagsManager.AddNativeGameplayTag(TEXT("Type.Cell.Storage"));
	GameplayTags.Type_Cell_Assemble_Wheel = TagsManager.AddNativeGameplayTag(TEXT("Type.Cell.Assemble.Wheel"));
	GameplayTags.Type_Cell_Assemble_Bumper = TagsManager.AddNativeGameplayTag(TEXT("Type.Cell.Assemble.Bumper"));
	GameplayTags.Type_Cell_Paint = TagsManager.AddNativeGameplayTag(TEXT("Type.Cell.Paint"));
	GameplayTags.Type_Cell_Inspection = TagsManager.AddNativeGameplayTag(TEXT("Type.Cell.Inspection"));
	GameplayTags.Trigger_Ability_Cell_AssignSpec = TagsManager.AddNativeGameplayTag(TEXT("Trigger.Ability.Cell.AssignSpec"));

	// --- 어빌리티 태그 ---
	GameplayTags.Ability_AGV_MoveToTarget = TagsManager.AddNativeGameplayTag(TEXT("Ability.AGV.MoveToTarget"));
	GameplayTags.Ability_Robot_PerformAssembly = TagsManager.AddNativeGameplayTag(TEXT("Ability.Robot.PerformAssembly"));
	GameplayTags.Ability_Cell_PerformInspection = TagsManager.AddNativeGameplayTag(TEXT("Ability.Cell.PerformInspection"));

	// --- 예지 보전 태그 ---
	GameplayTags.Event_Maintenance_Required = TagsManager.AddNativeGameplayTag(TEXT("Event.Maintenance.Required"));
}