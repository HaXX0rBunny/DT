#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "GameplayTagValidator.generated.h"

/**
 * GameplayTag 초기화 및 검증을 담당하는 엔진 서브시스템
 * 게임 시작 시 모든 필수 태그가 등록되어 있는지 확인
 */
UCLASS()
class DT_API UGameplayTagValidator : public UEngineSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Tag Validator")
    static bool ValidateAllTags();

    UFUNCTION(BlueprintCallable, Category = "Tag Validator")
    static void PrintAllRegisteredTags();

    UFUNCTION(BlueprintCallable, Category = "Tag Validator")
    static bool IsTagValid(FGameplayTag Tag);

    UFUNCTION(BlueprintCallable, Category = "Tag Validator")
    static TArray<FString> GetMissingTags();

private:
    static void ValidateCategoryTags(const FString& Category, TArray<FString>& OutMissingTags);
    static bool CheckTagExists(const FString& TagString);
};