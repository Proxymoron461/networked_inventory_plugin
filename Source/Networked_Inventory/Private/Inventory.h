// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Enums.h"
#include "Structs.h"
#include "Templates/Tuple.h"
#include "Inventory.generated.h"

/**
 * 
 */
UCLASS()
class UInventory : public UObject
{
	GENERATED_BODY()

private:
	
	UPROPERTY()
		TMap<FName, int32> InventoryEntries;

public:
	UInventory() : InventoryEntries(TMap<FName, int32>()) {}
	UInventory(const TMap<FName, int32>& entries) : InventoryEntries(entries) {}

	//UFUNCTION(Category = "Networked Inventory")  // FIXME: Unrecognised type 'TTuple' - type must be a UCLASS, USTRUCT, or UENUM
		TTuple<EChangeGroupStatus, TArray<EChangeStatus>> ModifyGroupOfEntries(const TArray<FInventoryEntry>& inventoryChanges);

	//UFUNCTION(Category = "Networked Inventory")
		TTuple<EChangeGroupStatus, TArray<EChangeStatus>> ModifyGroupOfEntries(const TMap<FName, int32>& inventoryChanges);

	UFUNCTION(Category = "Networked Inventory")
		EAddStatus AddNewEntry(const FInventoryEntry& entry);
		
	UFUNCTION(Category = "Networked Inventory")
		EChangeStatus ModifyEntry(const FInventoryEntry& entryChange);

	UFUNCTION(Category = "Networked Inventory")
		TArray<ERemovalStatus> RemoveGroupOfItems(const TArray<FName>& itemsToRemove);
	
	UFUNCTION(Category = "Networked Inventory")
		ERemovalStatus RemoveItem(const FName itemCode);

	UFUNCTION(Category = "Networked Inventory")
		int32 GetQuantityFor(const FName itemCode) const;

	UFUNCTION(Category = "Networked Inventory")
		bool Contains(const FName itemCode) const;

	UFUNCTION(BlueprintCallable, Category = "Networked Inventory")
		const TMap<FName, int32>& GetEntryMap() const;

	UFUNCTION(BlueprintCallable, Category = "Networked Inventory")
		int32 Num() const;

	UFUNCTION(BlueprintCallable, Category = "Networked Inventory")
		FString ToString() const;
};
