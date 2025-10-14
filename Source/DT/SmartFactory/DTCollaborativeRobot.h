
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DTCollaborativeRobot.generated.h"

class ASpec;
class USkeletalMeshComponent;
UCLASS()
class DT_API ADTCollaborativeRobot : public AFactoryEntityBase
{
    GENERATED_BODY()
public:
    // 로봇 팔 애니메이션, 특정 작업 로직 등을 위한 기능 포함
    UFUNCTION(BlueprintCallable, Category = "Collaborative Robot")
    void PerformWorkOnSpec(ASpec* TargetSpec, const FGameplayTag& WorkType);

    UFUNCTION(BlueprintCallable, Category = "Collaborative Robot")
    bool CanPerformWork(const FGameplayTag& WorkType) const;

    UFUNCTION(BlueprintCallable, Category = "Collaborative Robot")
    bool IsWorking() const;
};