

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/DT_DashboardWidget.h"
#include "DTPlayerController.generated.h"
class UDT_DashboardWidget;
/**
 * 
 */
UCLASS()
class DT_API ADTPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDT_DashboardWidget> DashboardWidgetClass;

	UPROPERTY()
	TObjectPtr<UDT_DashboardWidget> DashboardWidgetInstance;
private:
	
};
