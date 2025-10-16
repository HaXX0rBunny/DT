#pragma once

#include "CoreMinimal.h"
#include "Actor/InteractiveActorBase.h"
#include "Spec.generated.h"

class USpecAttributes;
class UDTSpecData;

UCLASS()
class DT_API ASpec : public AInteractiveActorBase
{
    GENERATED_BODY()

public:
    ASpec();
    virtual void BeginPlay() override;
    void InitializeSpec(UDTSpecData* SpecData);

protected:
    UPROPERTY(Transient)
    TObjectPtr<USpecAttributes> SpecAttributes;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> MeshComponent;
};