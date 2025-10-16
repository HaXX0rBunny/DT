#pragma once

#include "CoreMinimal.h"
#include "FactoryEntityBase.h"
#include "GameplayTagContainer.h"
#include "DTCollaborativeRobot.generated.h"

class ASpec;
class USkeletalMeshComponent;

UCLASS()
class DT_API ADTCollaborativeRobot : public AFactoryEntityBase
{
    GENERATED_BODY()

public:
    ADTCollaborativeRobot();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Collaborative Robot")
    void PerformWorkOnSpec(ASpec* TargetSpec, const FGameplayTag& WorkType);

    UFUNCTION(BlueprintCallable, Category = "Collaborative Robot")
    bool CanPerformWork(const FGameplayTag& WorkType) const;

    UFUNCTION(BlueprintCallable, Category = "Collaborative Robot")
    bool IsWorking() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USkeletalMeshComponent> RobotMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collaborative Robot")
    TArray<FGameplayTag> SupportedWorkTypes;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collaborative Robot")
    float WorkDuration = 3.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collaborative Robot")
    float WorkProgressPerCycle = 25.0f;

private:
    void CompleteWork();

    UPROPERTY()
    TObjectPtr<ASpec> CurrentWorkSpec;

    FTimerHandle WorkTimerHandle;
};