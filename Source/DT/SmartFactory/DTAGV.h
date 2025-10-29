#pragma once

#include "CoreMinimal.h"
#include "SmartFactory/FactoryEntityBase.h"
#include "GameplayTagContainer.h"
#include "DTAGV.generated.h"

class UAMR_Attributes;
class UFloatingPawnMovement;
class UAGVNavigationComponent;
class ASpec;

UCLASS()
class DT_API ADTAGV : public AFactoryEntityBase
{
    GENERATED_BODY()

public:
    ADTAGV();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "AGV")
    void AssignSpec(ASpec* NewSpec);

    UFUNCTION(BlueprintCallable, Category = "AGV")
    void ReleaseSpec();

    UFUNCTION(BlueprintCallable, Category = "AGV")
    bool HasSpec() const { return CurrentSpec != nullptr; }

    UFUNCTION(BlueprintCallable, Category = "AGV")
    ASpec* GetCurrentSpec() const { return CurrentSpec; }

    UFUNCTION(BlueprintCallable, Category = "AGV")
    bool MoveToDestination(const FGameplayTag& DestinationTag);

    UFUNCTION(BlueprintCallable, Category = "AGV")
    bool IsIdle() const;

    UFUNCTION(BlueprintCallable, Category = "AGV")
    UAGVNavigationComponent* GetNavigationComponent() const { return NavigationComponent; }

protected:
    UPROPERTY(Transient)
    TObjectPtr<UAMR_Attributes> AMRAttributes;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    TObjectPtr<UFloatingPawnMovement> MovementComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Navigation")
    TObjectPtr<UAGVNavigationComponent> NavigationComponent;

    UPROPERTY()
    TObjectPtr<ASpec> CurrentSpec;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGV")
    FName SpecAttachSocketName = TEXT("SpecSocket");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGV")
    float MovementSpeed = 300.0f;

private:
    UFUNCTION()
    void OnNavigationComplete();
};