// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryInterface.h"

// Add default functionality here for any IInventoryInterface functions that are not pure virtual.

void IInventoryInterface::AddItemToInventory(const FName itemCode, int32 quantity)
{
	FInventoryEntry entry;
	entry.ItemCode = itemCode;
	entry.Quantity = quantity;
	TArray<FInventoryEntry> arr;
	arr.Init(entry, 1);
	AddItemsToInventory(arr);
}
