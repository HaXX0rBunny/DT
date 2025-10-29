#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DTSyncActor.generated.h"

class IHttpRequest;
class IHttpResponse;
typedef TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> FHttpRequestPtr;
typedef TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> FHttpResponsePtr;

USTRUCT(BlueprintType)
struct FFactorySyncData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString ActorID;

    UPROPERTY(BlueprintReadOnly)
    float ProductionProgress = -1.f;

    UPROPERTY(BlueprintReadOnly)
    float BatteryLevel = -1.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFactoryDataReceived, const TArray<FFactorySyncData>&, ReceivedData);

UCLASS()
class DT_API ADTSyncActor : public AActor
{
    GENERATED_BODY()

public:
    ADTSyncActor();
    void RequestFactoryStatusUpdate();

    UPROPERTY(BlueprintAssignable, Category = "Digital Twin|API")
    FOnFactoryDataReceived OnFactoryDataReceived;

protected:
    UPROPERTY(EditAnywhere, Category = "Digital Twin|API")
    FString ApiEndpointUrl = TEXT("http://127.0.0.1:8000/factory/status");

private:
    void OnFactoryStatusResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};