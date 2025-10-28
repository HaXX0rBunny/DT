#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "DTFunctionLibrary.generated.h"

class AInteractiveActorBase;
class UAbilitySystemComponent;
class ASpec;
class ADTAGV;
class AFactoryCell;

/**
 * Digital Twin 시스템용 유틸리티 함수 라이브러리
 * Blueprint에서도 사용 가능한 헬퍼 함수들을 제공합니다
 */
UCLASS()
class DT_API UDTFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // ========================================
    // Gameplay Tag 헬퍼 함수
    // ========================================

    /** 액터가 특정 태그를 가지고 있는지 확인 */
    UFUNCTION(BlueprintPure, Category = "Digital Twin|Tags")
    static bool HasGameplayTag(AActor* Actor, FGameplayTag Tag);

    /** 액터에 태그 추가 */
    UFUNCTION(BlueprintCallable, Category = "Digital Twin|Tags")
    static void AddGameplayTag(AActor* Actor, FGameplayTag Tag);

    /** 액터에서 태그 제거 */
    UFUNCTION(BlueprintCallable, Category = "Digital Twin|Tags")
    static void RemoveGameplayTag(AActor* Actor, FGameplayTag Tag);

    // ========================================
    // Attribute 헬퍼 함수
    // ========================================

    /** 액터의 특정 Attribute 값 가져오기 */
    UFUNCTION(BlueprintPure, Category = "Digital Twin|Attributes")
    static float GetAttributeValue(AActor* Actor, FGameplayAttribute Attribute);

    /** 액터의 특정 Attribute 값 설정 */
    UFUNCTION(BlueprintCallable, Category = "Digital Twin|Attributes")
    static void SetAttributeValue(AActor* Actor, FGameplayAttribute Attribute, float NewValue);

    /** 배터리 레벨 가져오기 */
    UFUNCTION(BlueprintPure, Category = "Digital Twin|Attributes")
    static float GetBatteryLevel(AActor* Actor);

    /** 생산 진행률 가져오기 */
    UFUNCTION(BlueprintPure, Category = "Digital Twin|Attributes")
    static float GetProductionProgress(AActor* Actor);

    // ========================================
    // Factory 시스템 헬퍼 함수
    // ========================================

    /** 특정 타입의 Idle 상태인 Cell 찾기 */
    UFUNCTION(BlueprintCallable, Category = "Digital Twin|Factory", meta = (WorldContext = "WorldContextObject"))
    static AFactoryCell* FindIdleCell(const UObject* WorldContextObject, FGameplayTag CellTypeTag);

    /** Idle 상태인 AGV 찾기 */
    UFUNCTION(BlueprintCallable, Category = "Digital Twin|Factory", meta = (WorldContext = "WorldContextObject"))
    static ADTAGV* FindIdleAGV(const UObject* WorldContextObject);

    /** 특정 태그를 가진 모든 액터 찾기 */
    UFUNCTION(BlueprintCallable, Category = "Digital Twin|Factory", meta = (WorldContext = "WorldContextObject"))
    static TArray<AActor*> FindActorsWithTag(const UObject* WorldContextObject, FGameplayTag Tag);

    // ========================================
    // 수학 및 변환 함수
    // ========================================

    /** 진행률을 백분율 문자열로 변환 */
    UFUNCTION(BlueprintPure, Category = "Digital Twin|Utils")
    static FString ProgressToPercentString(float Progress);

    /** 배터리 레벨에 따른 색상 반환 (Low: Red, Medium: Yellow, High: Green) */
    UFUNCTION(BlueprintPure, Category = "Digital Twin|Utils")
    static FLinearColor GetBatteryLevelColor(float BatteryLevel);

    /** 마모도에 따른 색상 반환 */
    UFUNCTION(BlueprintPure, Category = "Digital Twin|Utils")
    static FLinearColor GetWearLevelColor(float WearLevel);

    // ========================================
    // Subsystem 접근 헬퍼
    // ========================================

    /** DT_DataManager 가져오기 */
    UFUNCTION(BlueprintPure, Category = "Digital Twin|Subsystems", meta = (WorldContext = "WorldContextObject"))
    static class UDT_DataManager* GetDataManager(const UObject* WorldContextObject);

    /** FactoryManagerSubsystem 가져오기 */
    UFUNCTION(BlueprintPure, Category = "Digital Twin|Subsystems", meta = (WorldContext = "WorldContextObject"))
    static class UFactoryManagerSubsystem* GetFactoryManager(const UObject* WorldContextObject);

    /** AGVManagerSubsystem 가져오기 */
    UFUNCTION(BlueprintPure, Category = "Digital Twin|Subsystems", meta = (WorldContext = "WorldContextObject"))
    static class UAGVManagerSubsystem* GetAGVManager(const UObject* WorldContextObject);

    /** MaintenanceManagerSubsystem 가져오기 */
    UFUNCTION(BlueprintPure, Category = "Digital Twin|Subsystems", meta = (WorldContext = "WorldContextObject"))
    static class UMaintenanceManagerSubsystem* GetMaintenanceManager(const UObject* WorldContextObject);

    // ========================================
    // 디버그 함수
    // ========================================

    /** 액터의 모든 Gameplay Tag 로그 출력 */
    UFUNCTION(BlueprintCallable, Category = "Digital Twin|Debug")
    static void DebugPrintGameplayTags(AActor* Actor);

    /** 액터의 모든 Attribute 로그 출력 */
    UFUNCTION(BlueprintCallable, Category = "Digital Twin|Debug")
    static void DebugPrintAttributes(AActor* Actor);

private:
    /** 액터로부터 AbilitySystemComponent 가져오기 */
    static UAbilitySystemComponent* GetASCFromActor(AActor* Actor);
};