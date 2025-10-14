

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FactoryLayoutManager.generated.h"

class UPCGComponent;

UCLASS()
class DT_API AFactoryLayoutManager : public AActor
{
	GENERATED_BODY()
	
public:
    AFactoryLayoutManager();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG")
    TObjectPtr<UPCGComponent> PCGComponent;

    UFUNCTION(BlueprintCallable, Category = "PCG")
    void GenerateFactoryLayout();
	
};
