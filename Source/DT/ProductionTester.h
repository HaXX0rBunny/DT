#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProductionTester.generated.h"

class AFactoryEntityBase;
class ASpec;


UCLASS()
class DT_API AProductionTester : public AActor
{
	GENERATED_BODY()

public:
	AProductionTester();

protected:
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Setup", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<ASpec> SpecActor;

private:

	void RunProductionTest();


	void RunAGVTest();


	void RunMaintenanceTest();

	UFUNCTION()
	void OnMaintenanceAlert(AFactoryEntityBase* Entity, float AlertValue);

};