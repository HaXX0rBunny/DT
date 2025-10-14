#include "SmartFactory/FactoryCell.h"
#include "SmartFactory/Spec.h"
#include "SmartFactory/DTCollaborativeRobot.h"
#include "SmartFactory/FactoryAttributeSet.h"
#include "GASCore/AssemblyLineAttributes.h"
#include "GASCore/SpecAttributes.h"
#include "AbilitySystemComponent.h"
#include "DataAssets/DTCellData.h"
#include "Tags/DTGameplayTags.h"
#include "Components/StaticMeshComponent.h"

AFactoryCell::AFactoryCell()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    // Root Component 설정
    USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    // AssemblyLine AttributeSet 생성
    AssemblyAttributes = CreateDefaultSubobject<UAssemblyLineAttributes>(TEXT("AssemblyAttributes"));
}

void AFactoryCell::BeginPlay()
{
    Super::BeginPlay();

    if (AbilitySystemComponent && AssemblyAttributes)
    {
        AbilitySystemComponent->AddAttributeSetSubobject(AssemblyAttributes.Get());

        AbilitySystemComponent->InitStats(UAssemblyLineAttributes::StaticClass(), nullptr);

        // 기본 값 설정
        AbilitySystemComponent->SetNumericAttributeBase(
            UAssemblyLineAttributes::GetProductionProgressAttribute(),
            0.0f
        );
        AbilitySystemComponent->SetNumericAttributeBase(
            UAssemblyLineAttributes::GetEfficiencyAttribute(),
            1.0f
        );
        AbilitySystemComponent->SetNumericAttributeBase(
            UAssemblyLineAttributes::GetWearAndTearAttribute(),
            0.0f
        );

        // 기본 Tags 추가
        const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();

        if (CellTypeTag.IsValid())
        {
            AbilitySystemComponent->AddLooseGameplayTag(CellTypeTag);
        }

        AbilitySystemComponent->AddLooseGameplayTag(GameplayTags.State_Cell_Idle);
    }
}

void AFactoryCell::InitializeCell(UDTCellData* CellData)
{
    if (!CellData || !AbilitySystemComponent)
    {
        return;
    }

    // Cell Type Tag 설정
    if (CellData->CellTypeTag.IsValid())
    {
        CellTypeTag = CellData->CellTypeTag;
        AbilitySystemComponent->AddLooseGameplayTag(CellData->CellTypeTag);
    }

    UE_LOG(LogTemp, Log, TEXT("FactoryCell %s initialized with type: %s"),
        *GetName(),
        *CellTypeTag.ToString()
    );
}

void AFactoryCell::AssignSpec(ASpec* NewSpec)
{
    if (!NewSpec || !AbilitySystemComponent)
    {
        return;
    }

    // 이미 작업 중이면 거부
    if (CurrentSpec)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cell %s is already processing a Spec!"), *GetName());
        return;
    }

    // Spec 할당
    CurrentSpec = NewSpec;

    // Spec을 작업 위치로 이동
    FVector WorldWorkLocation = GetActorLocation() + GetActorRotation().RotateVector(SpecWorkLocation);
    NewSpec->SetActorLocation(WorldWorkLocation);

    // 상태 변경
    const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();

    AbilitySystemComponent->RemoveLooseGameplayTag(GameplayTags.State_Cell_Idle);
    AbilitySystemComponent->AddLooseGameplayTag(GameplayTags.State_Cell_Progress);

    // 생산 진행률 초기화
    AbilitySystemComponent->SetNumericAttributeBase(
        UAssemblyLineAttributes::GetProductionProgressAttribute(),
        0.0f
    );

    OnWorkStart();

    UE_LOG(LogTemp, Log, TEXT("Cell %s assigned Spec %s"),
        *GetName(),
        *NewSpec->GetName()
    );
}

bool AFactoryCell::IsIdle() const
{
    if (!AbilitySystemComponent)
    {
        return false;
    }

    const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();
    return AbilitySystemComponent->HasMatchingGameplayTag(GameplayTags.State_Cell_Idle);
}

float AFactoryCell::GetProductionProgress() const
{
    if (!AbilitySystemComponent)
    {
        return 0.0f;
    }

    return AbilitySystemComponent->GetNumericAttribute(
        UAssemblyLineAttributes::GetProductionProgressAttribute()
    );
}

float AFactoryCell::GetEfficiency() const
{
    if (!AbilitySystemComponent)
    {
        return 1.0f;
    }

    return AbilitySystemComponent->GetNumericAttribute(
        UAssemblyLineAttributes::GetEfficiencyAttribute()
    );
}

void AFactoryCell::RegisterRobot(ADTCollaborativeRobot* Robot)
{
    if (!Robot || AssignedRobots.Contains(Robot))
    {
        return;
    }

    AssignedRobots.Add(Robot);

    UE_LOG(LogTemp, Log, TEXT("Cell %s registered Robot %s"),
        *GetName(),
        *Robot->GetName()
    );
}

void AFactoryCell::ResetCell()
{
    if (!AbilitySystemComponent)
    {
        return;
    }

    // 현재 Spec 해제
    CurrentSpec = nullptr;

    // 상태 초기화
    const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();

    AbilitySystemComponent->RemoveLooseGameplayTag(GameplayTags.State_Cell_Progress);
    AbilitySystemComponent->RemoveLooseGameplayTag(GameplayTags.State_Cell_Complete);
    AbilitySystemComponent->AddLooseGameplayTag(GameplayTags.State_Cell_Idle);

    // 진행률 초기화
    AbilitySystemComponent->SetNumericAttributeBase(
        UAssemblyLineAttributes::GetProductionProgressAttribute(),
        0.0f
    );

    UE_LOG(LogTemp, Log, TEXT("Cell %s reset to Idle state"), *GetName());
}

void AFactoryCell::OnWorkComplete()
{
    if (!AbilitySystemComponent)
    {
        return;
    }

    const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();

    // 상태 변경
    AbilitySystemComponent->RemoveLooseGameplayTag(GameplayTags.State_Cell_Progress);
    AbilitySystemComponent->AddLooseGameplayTag(GameplayTags.State_Cell_Complete);

    // 진행률 100%로 설정
    AbilitySystemComponent->SetNumericAttributeBase(
        UAssemblyLineAttributes::GetProductionProgressAttribute(),
        100.0f
    );

    // 사이클 카운트 증가 (예지보전용)
    if (AbilitySystemComponent->HasAttributeSetForAttribute(
        UFactoryAttributeSet::GetCycleCountAttribute()))
    {
        float CurrentCycles = AbilitySystemComponent->GetNumericAttribute(
            UFactoryAttributeSet::GetCycleCountAttribute()
        );
        AbilitySystemComponent->SetNumericAttributeBase(
            UFactoryAttributeSet::GetCycleCountAttribute(),
            CurrentCycles + 1.0f
        );
    }

    UE_LOG(LogTemp, Log, TEXT("Cell %s completed work on Spec %s"),
        *GetName(),
        CurrentSpec ? *CurrentSpec->GetName() : TEXT("None")
    );
}

void AFactoryCell::OnWorkStart()
{
    UE_LOG(LogTemp, Log, TEXT("Cell %s started work on Spec %s"),
        *GetName(),
        CurrentSpec ? *CurrentSpec->GetName() : TEXT("None")
    );

    // 작업 시작 시 할당된 로봇에게 작업 지시
    for (ADTCollaborativeRobot* Robot : AssignedRobots)
    {
        if (Robot && CurrentSpec && Robot->CanPerformWork(CellTypeTag))
        {
            // 로봇에게 작업 할당
            Robot->PerformWorkOnSpec(CurrentSpec, CellTypeTag);

            UE_LOG(LogTemp, Log, TEXT("Cell %s assigned work to Robot %s"),
                *GetName(),
                *Robot->GetName()
            );
        }
    }
}