#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "ChargeStation.generated.h"

class ADTAGV;
class UBoxComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChargingStarted, ADTAGV*, AGV, float, CurrentBatteryLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChargingComplete, ADTAGV*, AGV, float, FinalBatteryLevel);

UCLASS()
class DT_API AChargeStation : public AActor
{
    GENERATED_BODY()

public:
    AChargeStation();

    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Charge Station")
    bool IsAvailable() const { return CurrentChargingAGV == nullptr; }

    UFUNCTION(BlueprintCallable, Category = "Charge Station")
    bool StartCharging(ADTAGV* AGV);

    UFUNCTION(BlueprintCallable, Category = "Charge Station")
    void StopCharging();

    UFUNCTION(BlueprintPure, Category = "Charge Station")
    ADTAGV* GetCurrentChargingAGV() const { return CurrentChargingAGV; }

    UFUNCTION(BlueprintPure, Category = "Charge Station")
    FGameplayTag GetStationTag() const { return StationTag; }

    UPROPERTY(BlueprintAssignable, Category = "Charge Station")
    FOnChargingStarted OnChargingStarted;

    UPROPERTY(BlueprintAssignable, Category = "Charge Station")
    FOnChargingComplete OnChargingComplete;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USceneComponent> RootComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> StationMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxComponent> ChargingZone;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge Station")
    FGameplayTag StationTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge Station")
    float ChargeRate = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge Station")
    float FullChargeThreshold = 95.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge Station")
    bool bAutoStartCharging = true;

private:
    UPROPERTY()
    TObjectPtr<ADTAGV> CurrentChargingAGV;

    UFUNCTION()
    void OnChargingZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnChargingZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    void UpdateCharging(float DeltaTime);
    bool IsAGVFullyCharged(ADTAGV* AGV) const;
};