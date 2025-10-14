

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
    void RegisterAGV(ADTAGV* AGV);
    ADTAGV* GetIdleAGV();
    // AGV 할당, 경로 요청 등 DTAGVSystem 로직 담당
};
