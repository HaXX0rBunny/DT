// AProductionTester.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProductionTester.generated.h"

UCLASS()
class DT_API AProductionTester : public AActor
{
    GENERATED_BODY()

public:
    AProductionTester();

protected:
    virtual void BeginPlay() override;

private:
    void RunProductionTest();
    void RunAGVTest();
    void RunMaintenanceTest();
};