#include "DTLibrary/DTFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GASCore/AMR_Attributes.h"
#include "GASCore/AssemblyLineAttributes.h"
#include "GASCore/SpecAttributes.h"
#include "SmartFactory/FactoryAttributeSet.h"
#include "SmartFactory/FactoryCell.h"
#include "SmartFactory/DTAGV.h"
#include "Instance/DT_DataManager.h"
#include "Subsystem/FactoryManagerSubsystem.h"
#include "Subsystem/AGVManagerSubsystem.h"
#include "Subsystem/MaintenanceManagerSubsystem.h"
#include "Tags/DTGameplayTags.h"
#include "EngineUtils.h"
#include "Engine/World.h"

// ========================================
// Gameplay Tag 함수
// ========================================

bool UDTFunctionLibrary::HasGameplayTag(AActor* Actor, FGameplayTag Tag)
{
    UAbilitySystemComponent* ASC = GetASCFromActor(Actor);
    if (!ASC || !Tag.IsValid())
    {
        return false;
    }

    return ASC->HasMatchingGameplayTag(Tag);
}

void UDTFunctionLibrary::AddGameplayTag(AActor* Actor, FGameplayTag Tag)
{
    UAbilitySystemComponent* ASC = GetASCFromActor(Actor);
    if (ASC && Tag.IsValid())
    {
        ASC->AddLooseGameplayTag(Tag);
    }
}

void UDTFunctionLibrary::RemoveGameplayTag(AActor* Actor, FGameplayTag Tag)
{
    UAbilitySystemComponent* ASC = GetASCFromActor(Actor);
    if (ASC && Tag.IsValid())
    {
        ASC->RemoveLooseGameplayTag(Tag);
    }
}

// ========================================
// Attribute 함수
// ========================================

float UDTFunctionLibrary::GetAttributeValue(AActor* Actor, FGameplayAttribute Attribute)
{
    UAbilitySystemComponent* ASC = GetASCFromActor(Actor);
    if (!ASC || !Attribute.IsValid())
    {
        return 0.0f;
    }

    return ASC->GetNumericAttribute(Attribute);
}

void UDTFunctionLibrary::SetAttributeValue(AActor* Actor, FGameplayAttribute Attribute, float NewValue)
{
    UAbilitySystemComponent* ASC = GetASCFromActor(Actor);
    if (ASC && Attribute.IsValid())
    {
        ASC->SetNumericAttributeBase(Attribute, NewValue);
    }
}

float UDTFunctionLibrary::GetBatteryLevel(AActor* Actor)
{
    return GetAttributeValue(Actor, UAMR_Attributes::GetBatteryLevelAttribute());
}

float UDTFunctionLibrary::GetProductionProgress(AActor* Actor)
{
    UAbilitySystemComponent* ASC = GetASCFromActor(Actor);
    if (!ASC)
    {
        return 0.0f;
    }

    // AssemblyLine 또는 Spec의 Progress 확인
    if (ASC->HasAttributeSetForAttribute(UAssemblyLineAttributes::GetProductionProgressAttribute()))
    {
        return ASC->GetNumericAttribute(UAssemblyLineAttributes::GetProductionProgressAttribute());
    }
    else if (ASC->HasAttributeSetForAttribute(USpecAttributes::GetWorkProgressAttribute()))
    {
        return ASC->GetNumericAttribute(USpecAttributes::GetWorkProgressAttribute());
    }

    return 0.0f;
}

// ========================================
// Factory 시스템 함수
// ========================================

AFactoryCell* UDTFunctionLibrary::FindIdleCell(const UObject* WorldContextObject, FGameplayTag CellTypeTag)
{
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        return nullptr;
    }

    const FDTGameplayTags& GameplayTags = FDTGameplayTags::Get();

    for (TActorIterator<AFactoryCell> It(World); It; ++It)
    {
        AFactoryCell* Cell = *It;
        if (!Cell)
        {
            continue;
        }

        UAbilitySystemComponent* ASC = Cell->GetAbilitySystemComponent();
        if (!ASC)
        {
            continue;
        }

        if (ASC->HasMatchingGameplayTag(CellTypeTag) &&
            ASC->HasMatchingGameplayTag(GameplayTags.State_Cell_Idle))
        {
            return Cell;
        }
    }

    return nullptr;
}

ADTAGV* UDTFunctionLibrary::FindIdleAGV(const UObject* WorldContextObject)
{
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        return nullptr;
    }

    for (TActorIterator<ADTAGV> It(World); It; ++It)
    {
        ADTAGV* AGV = *It;
        if (AGV && AGV->IsIdle() && !AGV->HasSpec())
        {
            return AGV;
        }
    }

    return nullptr;
}

TArray<AActor*> UDTFunctionLibrary::FindActorsWithTag(const UObject* WorldContextObject, FGameplayTag Tag)
{
    TArray<AActor*> Result;

    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World || !Tag.IsValid())
    {
        return Result;
    }

    for (TActorIterator<AActor> It(World); It; ++It)
    {
        if (HasGameplayTag(*It, Tag))
        {
            Result.Add(*It);
        }
    }

    return Result;
}

// ========================================
// 수학 및 변환 함수
// ========================================

FString UDTFunctionLibrary::ProgressToPercentString(float Progress)
{
    return FString::Printf(TEXT("%.1f%%"), FMath::Clamp(Progress, 0.0f, 100.0f));
}

FLinearColor UDTFunctionLibrary::GetBatteryLevelColor(float BatteryLevel)
{
    if (BatteryLevel > 60.0f)
    {
        return FLinearColor::Green;
    }
    else if (BatteryLevel > 30.0f)
    {
        return FLinearColor::Yellow;
    }
    else
    {
        return FLinearColor::Red;
    }
}

FLinearColor UDTFunctionLibrary::GetWearLevelColor(float WearLevel)
{
    if (WearLevel < 0.5f)
    {
        return FLinearColor::Green;
    }
    else if (WearLevel < 0.8f)
    {
        return FLinearColor::Yellow;
    }
    else
    {
        return FLinearColor::Red;
    }
}

// ========================================
// Subsystem 접근 함수
// ========================================

UDT_DataManager* UDTFunctionLibrary::GetDataManager(const UObject* WorldContextObject)
{
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        return nullptr;
    }

    UGameInstance* GameInstance = World->GetGameInstance();
    if (!GameInstance)
    {
        return nullptr;
    }

    return GameInstance->GetSubsystem<UDT_DataManager>();
}

UFactoryManagerSubsystem* UDTFunctionLibrary::GetFactoryManager(const UObject* WorldContextObject)
{
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    return World ? World->GetSubsystem<UFactoryManagerSubsystem>() : nullptr;
}

UAGVManagerSubsystem* UDTFunctionLibrary::GetAGVManager(const UObject* WorldContextObject)
{
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    return World ? World->GetSubsystem<UAGVManagerSubsystem>() : nullptr;
}

UMaintenanceManagerSubsystem* UDTFunctionLibrary::GetMaintenanceManager(const UObject* WorldContextObject)
{
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    return World ? World->GetSubsystem<UMaintenanceManagerSubsystem>() : nullptr;
}

// ========================================
// 디버그 함수
// ========================================

void UDTFunctionLibrary::DebugPrintGameplayTags(AActor* Actor)
{
    UAbilitySystemComponent* ASC = GetASCFromActor(Actor);
    if (!ASC)
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor %s has no AbilitySystemComponent"), *Actor->GetName());
        return;
    }

    FGameplayTagContainer OwnedTags;
    ASC->GetOwnedGameplayTags(OwnedTags);

    UE_LOG(LogTemp, Log, TEXT("=== Gameplay Tags for %s ==="), *Actor->GetName());
    for (const FGameplayTag& Tag : OwnedTags)
    {
        UE_LOG(LogTemp, Log, TEXT("  - %s"), *Tag.ToString());
    }
    UE_LOG(LogTemp, Log, TEXT("================================"));
}

void UDTFunctionLibrary::DebugPrintAttributes(AActor* Actor)
{
    UAbilitySystemComponent* ASC = GetASCFromActor(Actor);
    if (!ASC)
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor %s has no AbilitySystemComponent"), *Actor->GetName());
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("=== Attributes for %s ==="), *Actor->GetName());

    // AMR Attributes
    if (ASC->HasAttributeSetForAttribute(UAMR_Attributes::GetBatteryLevelAttribute()))
    {
        UE_LOG(LogTemp, Log, TEXT("  BatteryLevel: %.2f"),
            ASC->GetNumericAttribute(UAMR_Attributes::GetBatteryLevelAttribute()));
        UE_LOG(LogTemp, Log, TEXT("  MaxBatteryLevel: %.2f"),
            ASC->GetNumericAttribute(UAMR_Attributes::GetMaxBatteryLevelAttribute()));
        UE_LOG(LogTemp, Log, TEXT("  CurrentLoad: %.2f"),
            ASC->GetNumericAttribute(UAMR_Attributes::GetCurrentLoadAttribute()));
    }

    // Assembly Attributes
    if (ASC->HasAttributeSetForAttribute(UAssemblyLineAttributes::GetProductionProgressAttribute()))
    {
        UE_LOG(LogTemp, Log, TEXT("  ProductionProgress: %.2f"),
            ASC->GetNumericAttribute(UAssemblyLineAttributes::GetProductionProgressAttribute()));
        UE_LOG(LogTemp, Log, TEXT("  Efficiency: %.2f"),
            ASC->GetNumericAttribute(UAssemblyLineAttributes::GetEfficiencyAttribute()));
        UE_LOG(LogTemp, Log, TEXT("  WearAndTear: %.2f"),
            ASC->GetNumericAttribute(UAssemblyLineAttributes::GetWearAndTearAttribute()));
    }

    // Spec Attributes
    if (ASC->HasAttributeSetForAttribute(USpecAttributes::GetWorkProgressAttribute()))
    {
        UE_LOG(LogTemp, Log, TEXT("  WorkProgress: %.2f"),
            ASC->GetNumericAttribute(USpecAttributes::GetWorkProgressAttribute()));
    }

    // Factory Attributes
    if (ASC->HasAttributeSetForAttribute(UFactoryAttributeSet::GetWearLevelAttribute()))
    {
        UE_LOG(LogTemp, Log, TEXT("  WearLevel: %.2f"),
            ASC->GetNumericAttribute(UFactoryAttributeSet::GetWearLevelAttribute()));
        UE_LOG(LogTemp, Log, TEXT("  CycleCount: %.2f"),
            ASC->GetNumericAttribute(UFactoryAttributeSet::GetCycleCountAttribute()));
    }

    UE_LOG(LogTemp, Log, TEXT("================================"));
}

// ========================================
// Private 함수
// ========================================

UAbilitySystemComponent* UDTFunctionLibrary::GetASCFromActor(AActor* Actor)
{
    if (!Actor)
    {
        return nullptr;
    }

    IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Actor);
    if (!ASI)
    {
        return nullptr;
    }

    return ASI->GetAbilitySystemComponent();
}