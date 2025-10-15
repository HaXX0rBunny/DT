

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "AGVManagerSubsystem.generated.h"

class ADTAGV;
class ASpec;

USTRUCT()
struct FAGVTask
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<ADTAGV> AssignedAGV;

    UPROPERTY()
    TObjectPtr<ASpec> TargetSpec;

    UPROPERTY()
    FGameplayTag DestinationTag;

    UPROPERTY()
    bool bIsCompleted = false;
};
UCLASS()
class DT_API UAGVManagerSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
    UFUNCTION(BlueprintCallable, Category ="AGV Manager")
    void RegisterAGV(ADTAGV* AGV);
    UFUNCTION(BlueprintCallable, Category ="AGV Manager")
    ADTAGV* GetIdleAGV();
    UFUNCTION(BlueprintCallable, Category = "AGV Manager")
    bool RequestAGVTransport(ASpec* Spec, const FGameplayTag& DestinationTag);

    UFUNCTION(BlueprintCallable, Category = "AGV Manager")
    int32 GetActiveTaskCount() const { return ActiveTasks.Num(); }
    // AGV 할당, 경로 요청 등 DTAGVSystem 로직 담당
private:
    void ScanForAGVs();
    void UpdateActiveTasks();
    void OnAGVMovementComplete(ADTAGV* AGV);

    UPROPERTY()
    TArray<TObjectPtr<ADTAGV>> RegisteredAGVs;

    UPROPERTY()
    TArray<FAGVTask> ActiveTasks;

    FTimerHandle UpdateTimerHandle;
};
