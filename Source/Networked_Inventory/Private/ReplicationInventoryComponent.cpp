// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplicationInventoryComponent.h"

void UReplicationInventoryComponent::RebuildCache()
{
	LookupCache.Empty();
	for (int32 i = 0; i < InventoryArray.Num(); i++)
	{
		LookupCache.Add(InventoryArray[i].ItemCode, i);
		check(InventoryArray[i] == InventoryArray[LookupCache[InventoryArray[i].ItemCode]]);
	}

	check(LookupCache.Num() == InventoryArray.Num());
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
	DOREPLIFETIME(UReplicationInventoryComponent, InventoryArray);
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

	InventoryArray.Add(entry);
	LookupCache[entry.ItemCode] = InventoryArray.Num() - 1;

	check(InventoryArray[LookupCache[entry.ItemCode]] == entry);

	return EAddStatus::Success;
}

EChangeStatus UReplicationInventoryComponent::ModifyEntry(const FInventoryEntry& entryChange)
{
	if (!Contains(entryChange.ItemCode))
	{
		InventoryArray.Emplace(entryChange.ItemCode, 0);
		LookupCache.Add(entryChange.ItemCode, InventoryArray.Num() - 1);
	}
	int32& quantityRef = InventoryArray[LookupCache[entryChange.ItemCode]].Quantity;
	quantityRef += entryChange.Quantity;

	check(quantityRef == InventoryArray[LookupCache[entryChange.ItemCode]].Quantity);

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
		InventoryArray.RemoveAt(LookupCache[itemCode]);  // Exception coming from here, when using RemoveAtSwap()
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
		return InventoryArray[LookupCache[itemCode]].Quantity;
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
	check(static_cast<bool>(InventoryArray.FindByPredicate(pred)) == LookupCache.Contains(itemCode));
	);

	return LookupCache.Contains(itemCode);
}

int32 UReplicationInventoryComponent::Num() const
{
	check(InventoryArray.Num() == LookupCache.Num());
	return InventoryArray.Num();
}

void UReplicationInventoryComponent::ModifyInventory(const TArray<FInventoryEntry>& inventoryChanges)
{
	ModifyGroupOfEntries(inventoryChanges);
}

FString UReplicationInventoryComponent::ToString() const
{
	FString s = "{\n";
	for (const auto& entry : InventoryArray)
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