// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplicationInventoryComponent.h"

void UReplicationInventoryComponent::RebuildCache()
{
	mLookupCache.Empty();
	for (int32 i = 0; i < mInventoryArray.Num(); i++)
	{
		mLookupCache.Add(mInventoryArray[i].ItemCode, i);
		check(mInventoryArray[i] == mInventoryArray[mLookupCache[mInventoryArray[i].ItemCode]]);
	}

	check(mLookupCache.Num() == mInventoryArray.Num());
}

UReplicationInventoryComponent::UReplicationInventoryComponent()
{
	// No need to tick.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UReplicationInventoryComponent::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UReplicationInventoryComponent, mInventoryArray);
}

TTuple<EChangeGroupStatus, TArray<EChangeStatus>> UReplicationInventoryComponent::ModifyGroupOfEntries(const TArray<FInventoryEntry>& inventoryChanges)
{
	TArray<EChangeStatus> changeStatuses;
	changeStatuses.Reserve(inventoryChanges.Num());

	EChangeGroupStatus groupStatus = EChangeGroupStatus::AllSuccessful;

	for (const auto& entry : inventoryChanges)
	{
		EChangeStatus status = ModifyEntry(entry);

		if (status != EChangeStatus::Success)
		{
			groupStatus = EChangeGroupStatus::SomeChangesLost;
		}


		changeStatuses.Add(status);
	}

	return TTuple<EChangeGroupStatus, TArray<EChangeStatus>>(groupStatus, changeStatuses);
}

TTuple<EChangeGroupStatus, TArray<EChangeStatus>> UReplicationInventoryComponent::ModifyGroupOfEntries(const TMap<FName, int32>& inventoryChanges)
{
	TArray<EChangeStatus> changeStatuses;
	changeStatuses.Reserve(inventoryChanges.Num());

	EChangeGroupStatus groupStatus = EChangeGroupStatus::AllSuccessful;

	for (const auto& pair : inventoryChanges)
	{
		FInventoryEntry entry;
		entry.ItemCode = pair.Key;
		entry.Quantity = pair.Value;
		EChangeStatus status = ModifyEntry(entry);

		if (status != EChangeStatus::Success)
		{
			groupStatus = EChangeGroupStatus::SomeChangesLost;
		}


		changeStatuses.Add(status);
	}

	return TTuple<EChangeGroupStatus, TArray<EChangeStatus>>(groupStatus, changeStatuses);
}

void UReplicationInventoryComponent::AddItemsToInventory(const TArray<FInventoryEntry>& inventoryChanges)
{
	checkCode(
		for (const FInventoryEntry& entry : inventoryChanges)
		{
			if (entry.Quantity <= 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Adding non-positive quantity of item %s. Quantity to add: %i"), *entry.ItemCode.ToString(), entry.Quantity);
			}
		}
	);
	ModifyGroupOfEntries(inventoryChanges);
}

void UReplicationInventoryComponent::RemoveItemsFromInventory(const TArray<FInventoryEntry>& inventoryChanges)
{
	TArray<FInventoryEntry> finalChanges;
	finalChanges.Reserve(inventoryChanges.Num());
	for (const FInventoryEntry& entry : inventoryChanges)
	{
		checkCode(
			if (entry.Quantity <= 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Removing non-positive quantity of item %s. Quantity to remove: %i"), *entry.ItemCode.ToString(), entry.Quantity);
			}
		);
		FInventoryEntry newEntry;
		newEntry.ItemCode = entry.ItemCode;
		newEntry.Quantity = -entry.Quantity;
		finalChanges.Add(newEntry);  // Flip so it is removed rather than added
	}

	ModifyGroupOfEntries(finalChanges);
}

EAddStatus UReplicationInventoryComponent::AddNewEntry(const FInventoryEntry& entry)
{
	if (Contains(entry.ItemCode))
	{
		return EAddStatus::ItemAlreadyInInventory;
	}

	mInventoryArray.Add(entry);
	mLookupCache[entry.ItemCode] = mInventoryArray.Num() - 1;

	check(mInventoryArray[mLookupCache[entry.ItemCode]] == entry);

	return EAddStatus::Success;
}

EChangeStatus UReplicationInventoryComponent::ModifyEntry(const FInventoryEntry& entryChange)
{
	if (!Contains(entryChange.ItemCode))
	{
		mInventoryArray.Emplace(entryChange.ItemCode, 0);
		mLookupCache.Add(entryChange.ItemCode, mInventoryArray.Num() - 1);
	}
	int32& quantityRef = mInventoryArray[mLookupCache[entryChange.ItemCode]].Quantity;
	quantityRef += entryChange.Quantity;

	check(quantityRef == mInventoryArray[mLookupCache[entryChange.ItemCode]].Quantity);

	if (quantityRef <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Non-positive quantity for item %s. Quantity: %i. Removing..."), *entryChange.ItemCode.ToString(), entryChange.Quantity);

		ERemovalStatus status = RemoveItem(entryChange.ItemCode);
		if (status != ERemovalStatus::Success)
		{
			return EChangeStatus::CouldNotMakeChange;
		}
	}

	return EChangeStatus::Success;
}

TArray<ERemovalStatus> UReplicationInventoryComponent::RemoveGroupOfItems(const TArray<FName>& itemsToRemove)
{
	TArray<ERemovalStatus> removalStatuses;
	removalStatuses.Reserve(itemsToRemove.Num());

	for (const FName& itemCode : itemsToRemove)
	{
		ERemovalStatus removalStatus = RemoveItem(itemCode);
		removalStatuses.Add(removalStatus);
	}

	return removalStatuses;
}

ERemovalStatus UReplicationInventoryComponent::RemoveItem(const FName itemCode)
{
	if (Contains(itemCode))
	{
		mInventoryArray.RemoveAt(mLookupCache[itemCode]);  // Exception coming from here, when using RemoveAtSwap()
		RebuildCache();  // Rebuild the cache entries
		return ERemovalStatus::Success;
	}
	else
	{
		return ERemovalStatus::ItemNotInInventory;
	}
}

int32 UReplicationInventoryComponent::GetQuantityFor(const FName itemCode) const
{
	if (Contains(itemCode))
	{
		return mInventoryArray[mLookupCache[itemCode]].Quantity;
	}
	else
	{
		return 0;
	}
}

bool UReplicationInventoryComponent::Contains(const FName itemCode) const
{
	checkCode(
		auto pred = [&itemCode](const FInventoryEntry& entry)
		{
			return entry.ItemCode == itemCode;
		};
	check(static_cast<bool>(mInventoryArray.FindByPredicate(pred)) == mLookupCache.Contains(itemCode));
	);

	return mLookupCache.Contains(itemCode);
}

int32 UReplicationInventoryComponent::Num() const
{
	check(mInventoryArray.Num() == mLookupCache.Num());
	return mInventoryArray.Num();
}

void UReplicationInventoryComponent::ModifyInventory(const TArray<FInventoryEntry>& inventoryChanges)
{
	ModifyGroupOfEntries(inventoryChanges);
}

FString UReplicationInventoryComponent::ToString() const
{
	FString s = "{\n";
	for (const auto& entry : mInventoryArray)
	{
		s.Appendf(TEXT("\t%s: %i\n"), *entry.ItemCode.ToString(), entry.Quantity);
	}
	s.Append(TEXT("}\n"));
	return s;
}

void UReplicationInventoryComponent::OnRep_InventoryArray()
{
	UE_LOG(LogTemp, Log, TEXT("Received new value for inventory array!"));
	RebuildCache();
}

void UReplicationInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}