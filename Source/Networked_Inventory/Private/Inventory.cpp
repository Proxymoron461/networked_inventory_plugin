// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

TTuple<EChangeGroupStatus, TArray<EChangeStatus>> UInventory::ModifyGroupOfEntries(const TArray<FInventoryEntry>& inventoryChanges)
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

TTuple<EChangeGroupStatus, TArray<EChangeStatus>> UInventory::ModifyGroupOfEntries(const TMap<FName, int32>& inventoryChanges)
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

EAddStatus UInventory::AddNewEntry(const FInventoryEntry& entry)
{
	if (Contains(entry.ItemCode))
	{
		return EAddStatus::ItemAlreadyInInventory;
	}

	mInventoryEntries[entry.ItemCode] = entry.Quantity;

	return EAddStatus::Success;
}

EChangeStatus UInventory::ModifyEntry(const FInventoryEntry& entryChange)
{
	int32& quantityRef = mInventoryEntries.FindOrAdd(entryChange.ItemCode, 0);
	quantityRef += entryChange.Quantity;

	check(quantityRef == mInventoryEntries[entryChange.ItemCode]);

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

TArray<ERemovalStatus> UInventory::RemoveGroupOfItems(const TArray<FName>& itemsToRemove)
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

ERemovalStatus UInventory::RemoveItem(const FName itemCode)
{
	if (mInventoryEntries.Contains(itemCode))
	{
		mInventoryEntries.Remove(itemCode);
		return ERemovalStatus::Success;
	}
	else
	{
		return ERemovalStatus::ItemNotInInventory;
	}
}

int32 UInventory::GetQuantityFor(const FName itemCode) const
{
	return mInventoryEntries.FindRef(itemCode);  // TODO: Default value should be 0 - check!
}

bool UInventory::Contains(const FName itemCode) const
{
	return mInventoryEntries.Contains(itemCode);
}

const TMap<FName, int32>& UInventory::GetEntryMap() const
{
	return mInventoryEntries;
}

int32 UInventory::Num() const
{
	return mInventoryEntries.Num();
}

FString UInventory::ToString() const
{
	FString s = "{\n";
	for (const auto& pair : mInventoryEntries)
	{
		s.Appendf(TEXT("\t%s: %i\n"), *pair.Key.ToString(), pair.Value);
	}
	s.Append(TEXT("}\n"));
	return s;
}
