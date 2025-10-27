

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DT_DashboardWidget.generated.h"
/**
 * 
 */

class UTextBlock;
class UProgressBar;

UCLASS()
class DT_API UDT_DashboardWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, Category = "UI Update")
    void UpdateProductionProgress(const FString& CellID, float NewProgress);

    UFUNCTION(BlueprintImplementableEvent, Category = "UI Update")
    void UpdateBatteryLevel(const FString& RobotID, float NewLevel);

protected:
    virtual void NativeConstruct() override;


    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    TObjectPtr<UTextBlock> MonitoredCellIDText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    TObjectPtr<UProgressBar> ProductionProgressBar;
};
