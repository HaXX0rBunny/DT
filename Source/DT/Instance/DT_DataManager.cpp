


#include "Instance/DT_DataManager.h"
#include "EngineUtils.h"
#include "AbilitySystemComponent.h"
#include "Actor/InteractiveActorBase.h"
#include "GASCore/AssemblyLineAttributes.h"
#include "GASCore/AMR_Attributes.h"
#include "SmartFactory/Spec.h"
#include "DataAssets/DTSpecData.h"
#include "DataAssets/DTCellData.h"
#include "SmartFactory/DestinationPoint.h"
#include "APISynchronizaion/DTSyncActor.h"
#include "DataAssets/DTSpecDataRow.h"
#include "DataAssets/DTCellDataRow.h"

void UDT_DataManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LoadAllDataAssets();
    GetWorld()->GetTimerManager().SetTimer(ScanTimerHandle, this, &UDT_DataManager::ScanWorldForActors, 1.0f, false);
}

void UDT_DataManager::Deinitialize()
{
    GetWorld()->GetTimerManager().ClearTimer(ScanTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(PollTimerHandle);
    Super::Deinitialize();
}

void UDT_DataManager::LoadAllDataAssets()
{
    // Spec 데이터 테이블 로드 (TSoftObjectPtr 사용)
    if (UDataTable* LoadedSpecTable = SpecDataTablePath.LoadSynchronous())
    {
        LoadedSpecTable->ForeachRow<FDTSpecDataRow>(TEXT("UDT_DataManager::LoadAllDataAssets_Spec"), [this](const FName& Key, const FDTSpecDataRow& Row)
            {
                if (Row.SpecData && Row.SpecData->SpecTypeTag.IsValid())
                {
                    SpecDataMap.Add(Row.SpecData->SpecTypeTag, Row.SpecData);
                }
            });
    }

    // Cell 데이터 테이블 로드 (TSoftObjectPtr 사용)
    if (UDataTable* LoadedCellTable = CellDataTablePath.LoadSynchronous())
    {
        LoadedCellTable->ForeachRow<FDTCellDataRow>(TEXT("UDT_DataManager::LoadAllDataAssets_Cell"), [this](const FName& Key, const FDTCellDataRow& Row)
            {
                if (Row.CellData && Row.CellData->CellTypeTag.IsValid())
                {
                    CellDataMap.Add(Row.CellData->CellTypeTag, Row.CellData);
                }
            });
    }
}

void UDT_DataManager::ScanWorldForActors()
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (TActorIterator<ADTSyncActor> It(World); It; ++It)
    {
        SyncActor = *It;
        if (SyncActor)
        {
            SyncActor->OnFactoryDataReceived.AddDynamic(this, &UDT_DataManager::OnApiDataReceived);
            World->GetTimerManager().SetTimer(PollTimerHandle, this, &UDT_DataManager::PollFactoryStatus, 5.0f, true, 1.0f);
            break;
        }
    }

    for (TActorIterator<AInteractiveActorBase> It(World); It; ++It)
    {
        RegisterActor(*It);
    }

    for (TActorIterator<ADestinationPoint> It(World); It; ++It)
    {
        if (*It && (*It)->DestinationTag.IsValid())
        {
            DestinationPoints.Add((*It)->DestinationTag, *It);
        }
    }
}

void UDT_DataManager::RegisterActor(AInteractiveActorBase* Actor)
{
    if (!Actor) return;
    UAbilitySystemComponent* ASC = Actor->GetAbilitySystemComponent();
    if (!ASC) return;

    FString ActorID = Actor->GetName();

    if (ASC->HasAttributeSetForAttribute(UAssemblyLineAttributes::GetProductionProgressAttribute()))
    {
        FString HandleKey = ActorID + "_Prod";
        if (!BoundDelegateHandles.Contains(HandleKey))
        {
            FDelegateHandle Handle = ASC->GetGameplayAttributeValueChangeDelegate(UAssemblyLineAttributes::GetProductionProgressAttribute()).AddUObject(this, &UDT_DataManager::OnProductionProgressChangedCallback, ActorID);
            BoundDelegateHandles.Add(HandleKey, Handle);
        }
    }

    if (ASC->HasAttributeSetForAttribute(UAMR_Attributes::GetBatteryLevelAttribute()))
    {
        FString HandleKey = ActorID + "_Batt";
        if (!BoundDelegateHandles.Contains(HandleKey))
        {
            FDelegateHandle Handle = ASC->GetGameplayAttributeValueChangeDelegate(UAMR_Attributes::GetBatteryLevelAttribute()).AddUObject(this, &UDT_DataManager::OnBatteryLevelChangedCallback, ActorID);
            BoundDelegateHandles.Add(HandleKey, Handle);
        }
    }
}

ASpec* UDT_DataManager::CreateNewSpec(FGameplayTag SpecTypeTag, const FTransform& SpawnTransform)
{
    //if (SpecDataMap.Contains(SpecTypeTag))
    //{
    //    UDTSpecData* Data = SpecDataMap;
    //    UWorld* World = GetWorld();
    //    if (World && Data)
    //    {
    //        ASpec* NewSpec = World->SpawnActor<ASpec>(ASpec::StaticClass(), SpawnTransform);
    //        if (NewSpec)
    //        {
    //            NewSpec->InitializeSpec(Data);
    //            return NewSpec;
    //        }
    //    }
    //}
    //return nullptr;
    UDTSpecData* Data = SpecDataMap.FindRef(SpecTypeTag);
    UWorld* World = GetWorld();
    if (World && Data)
    {
        ASpec* NewSpec = World->SpawnActor<ASpec>(ASpec::StaticClass(), SpawnTransform);
        if (NewSpec)
        {
            NewSpec->InitializeSpec(Data);
            return NewSpec;
        }
    }
    return nullptr;
}

bool UDT_DataManager::GetDestinationLocation(FGameplayTag DestinationTag, FVector& OutLocation)
{
    //if (DestinationPoints.Contains(DestinationTag))
    //{
    //    OutLocation = DestinationPoints->GetActorLocation();
    //    return true;
    //}
    //return false;

    ADestinationPoint* FoundPoint = DestinationPoints.FindRef(DestinationTag);
    if (FoundPoint)
    {
        OutLocation = FoundPoint->GetActorLocation();
        return true;
    }
    return false;

}

void UDT_DataManager::PollFactoryStatus()
{
    if (SyncActor)
    {
        SyncActor->RequestFactoryStatusUpdate();
    }
}

void UDT_DataManager::OnApiDataReceived(const TArray<FFactorySyncData>& ReceivedData)
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (const FFactorySyncData& Data : ReceivedData)
    {
        for (TActorIterator<AInteractiveActorBase> It(World); It; ++It)
        {
            AInteractiveActorBase* FoundActor = *It;
            if (FoundActor && FoundActor->GetName() == Data.ActorID)
            {
                UAbilitySystemComponent* ASC = FoundActor->GetAbilitySystemComponent();
                if (ASC)
                {
                    if (Data.ProductionProgress >= 0.f)
                    {
                        ASC->SetNumericAttributeBase(UAssemblyLineAttributes::GetProductionProgressAttribute(), Data.ProductionProgress);
                    }
                    if (Data.BatteryLevel >= 0.f)
                    {
                        ASC->SetNumericAttributeBase(UAMR_Attributes::GetBatteryLevelAttribute(), Data.BatteryLevel);
                    }
                }
                break;
            }
        }
    }
}

void UDT_DataManager::OnProductionProgressChangedCallback(const FOnAttributeChangeData& Data, FString ActorID)
{
    OnProductionProgressChanged.Broadcast(ActorID, Data.NewValue);
}

void UDT_DataManager::OnBatteryLevelChangedCallback(const FOnAttributeChangeData& Data, FString ActorID)
{
    OnBatteryLevelChanged.Broadcast(ActorID, Data.NewValue);
}