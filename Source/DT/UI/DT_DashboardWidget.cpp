


#include "UI/DT_DashboardWidget.h"
#include "Instance/DT_DataManager.h"
#include "Engine/GameInstance.h"

void UDT_DashboardWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (UGameInstance* GI = GetGameInstance())
    {
        UDT_DataManager* DataManager = GI->GetSubsystem<UDT_DataManager>();
        if (DataManager)
        {
            DataManager->OnProductionProgressChanged.AddDynamic(this, &UDT_DashboardWidget::UpdateProductionProgress);
            DataManager->OnBatteryLevelChanged.AddDynamic(this, &UDT_DashboardWidget::UpdateBatteryLevel);

        }
    }
}

