#include "APISynchronizaion/DTSyncActor.h"
#include "Http.h"
#include "HttpModule.h"
#include "Json.h"
#include "JsonUtilities.h"

ADTSyncActor::ADTSyncActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ADTSyncActor::RequestFactoryStatusUpdate()
{
    FHttpModule& HttpModule = FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();

    Request->SetURL(ApiEndpointUrl);
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    Request->OnProcessRequestComplete().BindUObject(this, &ADTSyncActor::OnFactoryStatusResponseReceived);
    Request->ProcessRequest();
}

void ADTSyncActor::OnFactoryStatusResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("API Request failed!"));
        return;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        TArray<FFactorySyncData> ParsedData;
        const TArray<TSharedPtr<FJsonValue>>* DataArray;
        if (JsonObject->TryGetArrayField(TEXT("data"), DataArray))
        {
            for (const TSharedPtr<FJsonValue>& Value : *DataArray)
            {
                const TSharedPtr<FJsonObject>& ItemObject = Value->AsObject();
                if (ItemObject.IsValid())
                {
                    FFactorySyncData SyncData;
                    ItemObject->TryGetStringField(TEXT("actor_id"), SyncData.ActorID);
                    ItemObject->TryGetNumberField(TEXT("production_progress"), SyncData.ProductionProgress);
                    ItemObject->TryGetNumberField(TEXT("battery_level"), SyncData.BatteryLevel);
                    ParsedData.Add(SyncData);
                }
            }
        }

        OnFactoryDataReceived.Broadcast(ParsedData);
    }
}