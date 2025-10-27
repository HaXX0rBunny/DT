// AProductionTester.cpp
#include "ProductionTester.h"
#include "Subsystem/FactoryManagerSubsystem.h"
#include "Subsystem/AGVManagerSubsystem.h"
#include "Subsystem/MaintenanceManagerSubsystem.h"
#include "Tags/DTGameplayTags.h"
#include "TimerManager.h"

AProductionTester::AProductionTester()
{
    PrimaryActorTick.bCanEverTick = false;
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
}

void AProductionTester::RunAGVTest()
{
    UAGVManagerSubsystem* AGVMgr = GetWorld()->GetSubsystem<UAGVManagerSubsystem>();
    if (AGVMgr && SpecActor)  // SpecActor는 어딘가에서 참조 필요
    {
        FGameplayTag DestTag = FGameplayTag::RequestGameplayTag(TEXT("TargetPoint.Cell.Enterance"));
        AGVMgr->RequestAGVTransport(SpecActor, DestTag);

        UE_LOG(LogTemp, Log, TEXT("AGV transport test started!"));
    }
}

void AProductionTester::RunMaintenanceTest()
{
    UMaintenanceManagerSubsystem* MaintenanceMgr = GetWorld()->GetSubsystem<UMaintenanceManagerSubsystem>();
    if (MaintenanceMgr)
    {
        // Delegate 바인딩
        MaintenanceMgr->OnMaintenanceRequired.AddDynamic(this, AProductionTester::OnMaintenanceAlert);

        UE_LOG(LogTemp, Log, TEXT("Maintenance monitoring started!"));
    }
}