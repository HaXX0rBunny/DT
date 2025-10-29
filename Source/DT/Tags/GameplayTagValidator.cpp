#include "GameplayTagValidator.h"
#include "Tags/DTGameplayTags.h"
#include "GameplayTagsManager.h"

void UGameplayTagValidator::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UE_LOG(LogTemp, Log, TEXT("=== GameplayTag Validator Initializing ==="));
    if (!FModuleManager::Get().IsModuleLoaded("DT"))
    {
        UE_LOG(LogTemp, Error, TEXT("DT Module not loaded! Tags will not be initialized!"));
        return;
    }
    bool bAllTagsValid = ValidateAllTags();

    if (bAllTagsValid)
    {
        UE_LOG(LogTemp, Log, TEXT("✓ All GameplayTags validated successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("✗ Some GameplayTags are missing!"));

        TArray<FString> MissingTags = GetMissingTags();
        for (const FString& Tag : MissingTags)
        {
            UE_LOG(LogTemp, Error, TEXT("  Missing Tag: %s"), *Tag);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("=========================================="));
}

bool UGameplayTagValidator::ValidateAllTags()
{
    const FDTGameplayTags& Tags = FDTGameplayTags::Get();
    bool bAllValid = true;

    auto CheckTag = [&bAllValid](const FGameplayTag& Tag, const FString& TagName)
        {
            if (!Tag.IsValid())
            {
                UE_LOG(LogTemp, Error, TEXT("Invalid Tag: %s"), *TagName);
                bAllValid = false;
                return false;
            }
            return true;
        };

    CheckTag(Tags.State_Idle, TEXT("State.Idle"));
    CheckTag(Tags.State_Busy, TEXT("State.Busy"));
    CheckTag(Tags.State_Error, TEXT("State.Error"));

    CheckTag(Tags.Type_AGV, TEXT("Type.AGV"));
    CheckTag(Tags.Type_AMR, TEXT("Type.AMR"));

    CheckTag(Tags.Type_Spec_ModelA, TEXT("Type.Spec.ModelA"));
    CheckTag(Tags.Type_Spec_ModelB, TEXT("Type.Spec.ModelB"));
    CheckTag(Tags.Type_Spec_ModelC, TEXT("Type.Spec.ModelC"));

    CheckTag(Tags.Type_Cell_Storage, TEXT("Type.Cell.Storage"));
    CheckTag(Tags.Type_Cell_Assemble_Wheel, TEXT("Type.Cell.Assemble.Wheel"));
    CheckTag(Tags.Type_Cell_Paint, TEXT("Type.Cell.Paint"));
    CheckTag(Tags.Type_Cell_Inspection, TEXT("Type.Cell.Inspection"));

    CheckTag(Tags.Event_Task_Complete, TEXT("Event.Task.Complete"));
    CheckTag(Tags.Event_AGV_MoveCompleted, TEXT("Event.AGV.MoveCompleted"));
    CheckTag(Tags.Event_Maintenance_Required, TEXT("Event.Maintenance.Required"));

    return bAllValid;
}

void UGameplayTagValidator::PrintAllRegisteredTags()
{
    UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();

    UE_LOG(LogTemp, Log, TEXT("=== All Registered GameplayTags ==="));

    FGameplayTagContainer AllTags;
    TagManager.RequestAllGameplayTags(AllTags, false);

    UE_LOG(LogTemp, Log, TEXT("Total Tags: %d"), AllTags.Num());

    TMap<FString, int32> CategoryCount;

    for (const FGameplayTag& Tag : AllTags)
    {
        FString TagString = Tag.ToString();
        UE_LOG(LogTemp, Log, TEXT("  - %s"), *TagString);

        int32 DotIndex;
        if (TagString.FindChar('.', DotIndex))
        {
            FString Category = TagString.Left(DotIndex);
            CategoryCount.FindOrAdd(Category)++;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("\n=== Tags by Category ==="));
    for (const auto& Pair : CategoryCount)
    {
        UE_LOG(LogTemp, Log, TEXT("%s: %d tags"), *Pair.Key, Pair.Value);
    }

    UE_LOG(LogTemp, Log, TEXT("==================================="));
}

bool UGameplayTagValidator::IsTagValid(FGameplayTag Tag)
{
    if (!Tag.IsValid())
    {
        return false;
    }

    UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();
    return TagManager.IsValidGameplayTagString(Tag.ToString());
}

TArray<FString> UGameplayTagValidator::GetMissingTags()
{
    TArray<FString> MissingTags;

    TArray<FString> ExpectedTags = {
        TEXT("State.Idle"),
        TEXT("State.Busy"),
        TEXT("State.Error"),
        TEXT("State.CellRobot.Idle"),
        TEXT("State.CellRobot.Working"),
        TEXT("State.AGV.Idle"),
        TEXT("State.AGV.MoveToCell"),
        TEXT("State.Spec.WorkProgress"),
        TEXT("State.Cell.Progress"),
        TEXT("Type.AGV"),
        TEXT("Type.AMR"),
        TEXT("Type.Spec.ModelA"),
        TEXT("Type.Cell.Assemble.Wheel"),
        TEXT("Event.Task.Complete"),
        TEXT("Event.AGV.MoveCompleted"),
        TEXT("Trigger.Ability.Cell.AssignSpec")
    };

    for (const FString& TagString : ExpectedTags)
    {
        if (!CheckTagExists(TagString))
        {
            MissingTags.Add(TagString);
        }
    }

    return MissingTags;
}

void UGameplayTagValidator::ValidateCategoryTags(const FString& Category, TArray<FString>& OutMissingTags)
{
    UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();

    FGameplayTagContainer AllTags;
    TagManager.RequestAllGameplayTags(AllTags, false);

    int32 CategoryTagCount = 0;
    for (const FGameplayTag& Tag : AllTags)
    {
        if (Tag.ToString().StartsWith(Category))
        {
            CategoryTagCount++;
        }
    }

    if (CategoryTagCount == 0)
    {
        OutMissingTags.Add(FString::Printf(TEXT("No tags found in category: %s"), *Category));
    }
}

bool UGameplayTagValidator::CheckTagExists(const FString& TagString)
{
    UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();
    FGameplayTag Tag = TagManager.RequestGameplayTag(FName(*TagString), false);
    return Tag.IsValid();
}