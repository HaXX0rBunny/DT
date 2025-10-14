

#include "Subsystem/FactoryManagerSubsystem.h"
#include "SmartFactory/FactoryEntityBase.h"
#include "SmartFactory/FactoryCell.h"
#include "SmartFactory/DTAGV.h"
#include "SmartFactory/Spec.h"
#include "Instance/DT_DataManager.h"
#include "AbilitySystemComponent.h"
#include "Tags/DTGameplayTags.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "TimerManager.h"


void UFactoryManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(this, &UFactoryManagerSubsystem::PopulateFactory);
	}
}

void UFactoryManagerSubsystem::Deinitialize()
{
	StopProduction();
	Super::Deinitialize();
}

void UFactoryManagerSubsystem::QueueProductionOrder(const FGameplayTag& ProductSpecTag, int32 Quantity, float Priority)
{
	if (!ProductSpecTag.IsValid() || Quantity <= 0)
		return;
	FProductionOrder Order;
	Order.ProductSpecTag = ProductSpecTag;
	Order.Quantity = Quantity;
	Order.Priority = Priority;

	ProductionOrderQueue.Enqueue(Order);

}



AFactoryEntityBase* UFactoryManagerSubsystem::FindAssetByTag(const FGameplayTag& AssetTag) const
{
	if (const TWeakObjectPtr<AFactoryEntityBase>* FoundAsset = AssetRegistry.Find(AssetTag))
	{
		return FoundAsset->Get();
	}
	return nullptr;
}

void UFactoryManagerSubsystem::StartProduction()
{
	if (bIsProducing)
		return;
	bIsProducing = true;
	ProcessProductionQueue();
}

void UFactoryManagerSubsystem::StopProduction()
{
	bIsProducing = false;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ProductionTimerHandle);
	}
}

void UFactoryManagerSubsystem::PopulateFactory()
{
	UWorld* World = GetWorld();
	if (!World)
		return;
	AvailableCells.Empty();
	AvailableAGVs.Empty();
	AssetRegistry.Empty();
	for (TActorIterator<AFactoryCell> It(World); It; ++It)
	{
		AFactoryCell* Cell = *It;
		if (Cell)
		{
			AvailableCells.Add(Cell);
			if (UAbilitySystemComponent* ASC = Cell->GetAbilitySystemComponent())
			{
				FGameplayTagContainer OwnedTags;
				ASC->GetOwnedGameplayTags(OwnedTags);

				for (const FGameplayTag& Tag : OwnedTags)
				{
					if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Type.Cell"))))
					{
						AssetRegistry.Add(Tag, Cell);
					}
				}
			}
		}
	}
}

void UFactoryManagerSubsystem::ProcessProductionQueue()
{
	if (!bIsProducing || ProductionOrderQueue.IsEmpty())
	{
		return;
	}
	FProductionOrder Order;
	if (!ProductionOrderQueue.Peek(Order))
		return;
	AssignSpecToCell(Order.ProductSpecTag);
	Order.Quantity--;
	if (Order.Quantity <= 0)
		ProductionOrderQueue.Pop();
}

void UFactoryManagerSubsystem::AssignSpecToCell(const FGameplayTag& SpecTag)
{
}

AFactoryCell* UFactoryManagerSubsystem::FindAvailableCall(const FGameplayTag& CellTypeTag)
{
	return nullptr;
}
