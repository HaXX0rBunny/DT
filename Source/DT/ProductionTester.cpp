#include "ProductionTester.h"
#include "Subsystem/FactoryManagerSubsystem.h" 
#include "Subsystem/AGVManagerSubsystem.h" 
#include "Subsystem/MaintenanceManagerSubsystem.h" 
#include "Tags/DTGameplayTags.h" 
#include "GameplayTagsManager.h"
#include "TimerManager.h"
#include "SmartFactory/Spec.h"
#include "SmartFactory/FactoryEntityBase.h"
#include "GameFramework/Actor.h" 

AProductionTester::AProductionTester()
{
	PrimaryActorTick.bCanEverTick = false;

	// SpecActor는 에디터에서 할당할 때까지 null입니다.
	SpecActor = nullptr;
}

void AProductionTester::BeginPlay()
{
	Super::BeginPlay();

	// 2초 후 테스트 시작
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&AProductionTester::RunProductionTest,
		2.0f,
		false
	);
}

void AProductionTester::RunProductionTest()
{
	UFactoryManagerSubsystem* FactoryMgr = GetWorld()->GetSubsystem<UFactoryManagerSubsystem>();
	if (FactoryMgr)
	{
		FGameplayTag SpecTag = FGameplayTag::RequestGameplayTag(TEXT("Type.Spec.ModelA"));
		FactoryMgr->QueueProductionOrder(SpecTag, 1, 1.0f);
		FactoryMgr->StartProduction();

		UE_LOG(LogTemp, Log, TEXT("Production test started!"));
	}

	// 다른 테스트들도 연달아 실행
	RunAGVTest();
	RunMaintenanceTest();
}

void AProductionTester::RunAGVTest()
{
	UAGVManagerSubsystem* AGVMgr = GetWorld()->GetSubsystem<UAGVManagerSubsystem>();

	// SpecActor가 유효한지(에디터에서 할당되었는지) 반드시 확인해야 합니다.
	if (AGVMgr && SpecActor)
	{
		FGameplayTag DestTag = FGameplayTag::RequestGameplayTag(TEXT("TargetPoint.Cell.Enterance"));
		AGVMgr->RequestAGVTransport(SpecActor, DestTag);

		UE_LOG(LogTemp, Log, TEXT("AGV transport test started for %s!"), *SpecActor->GetName());
	}
	else if (!SpecActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("AGV transport test skipped: SpecActor is not set in the editor."));
	}
}

void AProductionTester::RunMaintenanceTest()
{
	UMaintenanceManagerSubsystem* MaintenanceMgr = GetWorld()->GetSubsystem<UMaintenanceManagerSubsystem>();
	if (MaintenanceMgr)
	{
		// Delegate 바인딩
		MaintenanceMgr->OnMaintenanceRequired.AddDynamic(this, &AProductionTester::OnMaintenanceAlert);

		UE_LOG(LogTemp, Log, TEXT("Maintenance monitoring started!"));
	}
}


void AProductionTester::OnMaintenanceAlert(AFactoryEntityBase* Entity, float AlertValue)
{
	UE_LOG(LogTemp, Warning, TEXT("!!! MAINTENANCE ALERT RECEIVED !!!"));

	if (Entity)
	{
		UE_LOG(LogTemp, Log, TEXT("Alert triggered by Entity: %s, Value: %f"), *Entity->GetName(), AlertValue);
	}
	if (SpecActor)
	{

		if (UWorld* World = GetWorld())
		{
			UAGVManagerSubsystem* AGVMgr = World->GetSubsystem<UAGVManagerSubsystem>();
			if (AGVMgr)
			{

				FGameplayTag RepairStationTag = FGameplayTag::RequestGameplayTag(TEXT("TargetPoint.Maintenance.RepairBay"));

				AGVMgr->RequestAGVTransport(SpecActor, RepairStationTag);

				UE_LOG(LogTemp, Log, TEXT("Maintenance Alert: Rerouting '%s' to Repair Bay (%s)."),
					*SpecActor->GetName(), *RepairStationTag.ToString());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Maintenance Alert Received, but 'SpecActor' is not set. Cannot reroute for maintenance."));
	}
}