

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
    void UpdateProductionProgress(FString CellID, float NewProgress);

    UFUNCTION(BlueprintImplementableEvent, Category = "UI Update")
    void UpdateBatteryLevel(FString RobotID, float NewLevel);

protected:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> MonitoredCellIDText;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UProgressBar> ProductionProgressBar;
};
