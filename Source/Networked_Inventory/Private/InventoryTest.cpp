#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#include "Inventory.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryContainsAddedItems, "Inventory.Contains Added Items", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FInventoryContainsAddedItems::RunTest(const FString& Parameters)
{
	UInventory* inventory;
	inventory = NewObject<UInventory>();

	for (int i = 0; i < 100; i++)
	{
		int32 num = FMath::RandRange(1, 1000);
		FInventoryEntry entry;
		entry.ItemCode = FName(*FString::FromInt(num));
		entry.Quantity = num;
		inventory->ModifyEntry(entry);

		if (!inventory->Contains(entry.ItemCode))
		{
			AddError(FString::Printf(TEXT("Inventory does not contain item %s, despite adding positive amount. Amount added: %i"), *entry.ItemCode.ToString(), entry.Quantity));
		}
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryDoesNotHaveEntryWhenQuantityIsNegative, "Inventory.No Entry For Negative Quantities", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FInventoryDoesNotHaveEntryWhenQuantityIsNegative::RunTest(const FString& Parameters)
{
	UInventory* inventory;
	inventory = NewObject<UInventory>();

	for (int i = 0; i < 100; i++)
	{
		int32 num = FMath::RandRange(-1000, -1);
		FInventoryEntry entry;
		entry.ItemCode = FName(*FString::FromInt(num));
		entry.Quantity = num;
		inventory->ModifyEntry(entry);

		if (inventory->Contains(entry.ItemCode))
		{
			AddError(FString::Printf(TEXT("Inventory contains item %s, despite adding negative amount. Amount added: %i"), *entry.ItemCode.ToString(), entry.Quantity));
		}
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryDoesNotHaveEntryWhenQuantityIsZero, "Inventory.No Entry For Quantities Of Zero", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FInventoryDoesNotHaveEntryWhenQuantityIsZero::RunTest(const FString& Parameters)
{
	UInventory* inventory;
	inventory = NewObject<UInventory>();

	for (int i = 0; i < 100; i++)
	{
		FInventoryEntry entry;
		entry.ItemCode = FName(*FString::FromInt(FMath::RandRange(1, 1000)));
		entry.Quantity = 0;
		inventory->ModifyEntry(entry);

		if (inventory->Contains(entry.ItemCode))
		{
			AddError(FString::Printf(TEXT("Inventory contains item %s, despite adding zero quantity."), *entry.ItemCode.ToString()));
		}
	}

	return (inventory->Num() == 0);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryDoesNotContainRemovedItems, "Inventory.Does Not Contain Removed Items", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FInventoryDoesNotContainRemovedItems::RunTest(const FString& Parameters)
{
	UInventory* inventory;
	inventory = NewObject<UInventory>();

	for (int i = 0; i < 100; i++)
	{
		int32 num = FMath::RandRange(1, 1000);
		FInventoryEntry entry;
		entry.ItemCode = FName(*FString::FromInt(num));
		entry.Quantity = num;
		inventory->ModifyEntry(entry);

		if (!inventory->Contains(entry.ItemCode))
		{
			AddError(FString::Printf(TEXT("Inventory does not contain item %s, despite adding positive amount. Amount added: %i"), *entry.ItemCode.ToString(), entry.Quantity));
		}

		inventory->RemoveItem(entry.ItemCode);

		if (inventory->Contains(entry.ItemCode))
		{
			AddError(FString::Printf(TEXT("Inventory contains item %s, despite it being removed."), *entry.ItemCode.ToString()));
		}
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryCanAddQuantityToEntries, "Inventory.Can Add Quantity To Entries", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FInventoryCanAddQuantityToEntries::RunTest(const FString& Parameters)
{
	UInventory* inventory;
	inventory = NewObject<UInventory>();

	for (int i = 0; i < 100; i++)
	{
		int32 num = FMath::RandRange(1, 1000);
		FInventoryEntry entry;
		entry.ItemCode = FName(*FString::FromInt(num));
		entry.Quantity = num;
		inventory->ModifyEntry(entry);

		int32 quantityBeforeIncrease = inventory->GetQuantityFor(entry.ItemCode);
		entry.Quantity = FMath::RandRange(1, 1000);
		inventory->ModifyEntry(entry);

		if (inventory->GetQuantityFor(entry.ItemCode) <= quantityBeforeIncrease)
		{
			AddError(FString::Printf(TEXT("Despite adding positive quantity, amount in inventory has not increased. Quantity before increase: %i, Amount increased by: %i, New quantity: %i"), quantityBeforeIncrease, entry.Quantity, inventory->GetQuantityFor(entry.ItemCode)));
		}
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryCanRemoveQuantityFromEntries, "Inventory.Can Remove Quantity From Entries", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FInventoryCanRemoveQuantityFromEntries::RunTest(const FString& Parameters)
{
		UInventory* inventory;
		inventory = NewObject<UInventory>();

		for (int i = 0; i < 100; i++)
		{
			int32 num = FMath::RandRange(100, 1000);
			FInventoryEntry entry;
			entry.ItemCode = FName(*FString::FromInt(num));
			entry.Quantity = num;
			inventory->ModifyEntry(entry);

			int32 quantityBeforeDecrease = inventory->GetQuantityFor(entry.ItemCode);
			entry.Quantity = FMath::RandRange(-50, -10);
			inventory->ModifyEntry(entry);

			if (inventory->GetQuantityFor(entry.ItemCode) >= quantityBeforeDecrease)
			{
				AddError(FString::Printf(TEXT("Despite adding negative quantity (i.e. removing), amount in inventory has not decreased. Quantity before increase: %i, Amount changed by: %i, New quantity: %i"), quantityBeforeDecrease, entry.Quantity, inventory->GetQuantityFor(entry.ItemCode)));
			}

		}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryRemovingGreaterQuantityRemovesTheEntry, "Inventory.Removing Greater Quantity Removes Entry", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FInventoryRemovingGreaterQuantityRemovesTheEntry::RunTest(const FString& Parameters)
{
	UInventory* inventory;
	inventory = NewObject<UInventory>();

	for (int i = 0; i < 100; i++)
	{
		int32 num = FMath::RandRange(10, 50);
		FInventoryEntry entry;
		entry.ItemCode = FName(*FString::FromInt(num));
		entry.Quantity = num;
		inventory->ModifyEntry(entry);

		int32 quantityBeforeDecrease = inventory->GetQuantityFor(entry.ItemCode);
		entry.Quantity = FMath::RandRange(-1000, -100);
		inventory->ModifyEntry(entry);

		if (inventory->Contains(entry.ItemCode))
		{
			AddError(FString::Printf(TEXT("Despite adding negative quantity (i.e. removing) exceeding held quantity, entry %s has not been removed. Quantity before increase: %i, Amount changed by: %i"), *entry.ItemCode.ToString(), quantityBeforeDecrease, entry.Quantity));
		}

	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryRemoveItemsWipesEntries, "Inventory.Remove Items Wipes Entries", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FInventoryRemoveItemsWipesEntries::RunTest(const FString& Parameters)
{
	UInventory* inventory;
	inventory = NewObject<UInventory>();

	TArray<FName> items;

	for (int i = 0; i < 100; i++)
	{
		int32 num = FMath::RandRange(1, 1000);
		FInventoryEntry entry;
		entry.ItemCode = FName(*FString::FromInt(num));
		entry.Quantity = num;
		inventory->ModifyEntry(entry);

		items.Add(entry.ItemCode);
	}

	if (inventory->Num() <= 0)
	{
		AddError(TEXT("Despite adding plenty of items, the inventory is empty."));
	}

	inventory->RemoveGroupOfItems(items);

	for (const FName& name : items)
	{
		if (inventory->Contains(name))
		{
			AddError(FString::Printf(TEXT("Despite removal, inventory still contains an entry for item %s."), *name.ToString()));
		}
	}

	return inventory->Num() == 0;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryCanModifyEntriesInGroups, "Inventory.Can Modify Entries In Groups", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FInventoryCanModifyEntriesInGroups::RunTest(const FString& Parameters)
{
	UInventory* inventory;
	inventory = NewObject<UInventory>();

	TMap<FName, int32> changes;

	for (int i = 0; i < 100; i++)
	{
		int32 num = FMath::RandRange(1, 1000);
		FInventoryEntry entry;
		entry.ItemCode = FName(*FString::FromInt(num));
		entry.Quantity = num;
		inventory->ModifyEntry(entry);

		int32& quantityRef = changes.FindOrAdd(entry.ItemCode, 0);
		quantityRef += entry.Quantity;
	}

	UInventory* mapModifiedInventory;
	mapModifiedInventory = NewObject<UInventory>();

	mapModifiedInventory->ModifyGroupOfEntries(changes);

	for (const auto& pair : changes)
	{
		if (inventory->Contains(pair.Key) && !mapModifiedInventory->Contains(pair.Key))
		{
			AddError(FString::Printf(TEXT("Single-modified inventory contains entry for item %s, but map modified inventory does not."), *pair.Key.ToString()));
		}

		if (!inventory->Contains(pair.Key) && mapModifiedInventory->Contains(pair.Key))
		{
			AddError(FString::Printf(TEXT("Single-modified inventory contains no entry for item %s, but map modified inventory does."), *pair.Key.ToString()));
		}

		if (inventory->GetQuantityFor(pair.Key) != mapModifiedInventory->GetQuantityFor(pair.Key))
		{
			AddError(FString::Printf(TEXT("Quantity mismatch for item %s between inventories. Single-modified inventory quantity: %i, Map-modified inventory quantity: %i"), *pair.Key.ToString(), inventory->GetQuantityFor(pair.Key), mapModifiedInventory->GetQuantityFor(pair.Key)));
		}
	}

	UInventory* arrayModifiedInventory;
	arrayModifiedInventory = NewObject<UInventory>();

	TArray<FInventoryEntry> changesArray;
	for (const auto& pair : changes)
	{
		FInventoryEntry entry;
		entry.ItemCode = pair.Key;
		entry.Quantity = pair.Value;
		changesArray.Add(entry);
	}

	arrayModifiedInventory->ModifyGroupOfEntries(changesArray);

	for (const auto& pair : changes)
	{
		if (inventory->Contains(pair.Key) && !arrayModifiedInventory->Contains(pair.Key))
		{
			AddError(FString::Printf(TEXT("Single-modified inventory contains entry for item %s, but array modified inventory does not."), *pair.Key.ToString()));
		}

		if (!inventory->Contains(pair.Key) && arrayModifiedInventory->Contains(pair.Key))
		{
			AddError(FString::Printf(TEXT("Single-modified inventory contains no entry for item %s, but array modified inventory does."), *pair.Key.ToString()));
		}

		if (inventory->GetQuantityFor(pair.Key) != arrayModifiedInventory->GetQuantityFor(pair.Key))
		{
			AddError(FString::Printf(TEXT("Quantity mismatch for item %s between inventories. Single-modified inventory quantity: %i, Array-modified inventory quantity: %i"), *pair.Key.ToString(), inventory->GetQuantityFor(pair.Key), arrayModifiedInventory->GetQuantityFor(pair.Key)));
		}
	}

	return true;
}