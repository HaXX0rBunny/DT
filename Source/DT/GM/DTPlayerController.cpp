

#include "DTPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UI/DT_DashboardWidget.h"



void ADTPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalPlayerController() && DashboardWidgetClass)
    {
        DashboardWidgetInstance = CreateWidget<UDT_DashboardWidget>(this, DashboardWidgetClass);
        if (DashboardWidgetInstance)
        {
            DashboardWidgetInstance->AddToViewport();
        }
    }
}