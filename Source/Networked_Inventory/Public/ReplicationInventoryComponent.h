// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enums.h"
#include "Structs.h"
#include "CoreMinimal.h"
#include "Templates/Tuple.h"
#include "Net/UnrealNetwork.h"
#include "InventoryInterface.h"
#include "Components/ActorComponent.h"
#include "ReplicationInventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETWORKED_INVENTORY_API UReplicationInventoryComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()

private:

	UPROPERTY(ReplicatedUsing = OnRep_InventoryArray)
		TArray<FInventoryEntry> mInventoryArray;

	UPROPERTY()
		TMap<FName, int32> mLookupCache;

	UFUNCTION()
		void RebuildCache();

	UFUNCTION()
		void OnRep_InventoryArray();

public:
	UReplicationInventoryComponent();

	//UFUNCTION(Category = "Networked Inventory")  // FIXME: Unrecognised type 'TTuple' - type must be a UCLASS, USTRUCT, or UENUM
	TTuple<EChangeGroupStatus, TArray<EChangeStatus>> ModifyGroupOfEntries(const TArray<FInventoryEntry>& inventoryChanges);

	//UFUNCTION(Category = "Networked Inventory")
	TTuple<EChangeGroupStatus, TArray<EChangeStatus>> ModifyGroupOfEntries(const TMap<FName, int32>& inventoryChanges);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool IsSupportedForNetworking() const override { return true; }

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
		int32 Num() const;

	UFUNCTION(Category = "Networked Inventory")
		virtual void ModifyInventory(const TArray<FInventoryEntry>& inventoryChanges) override;

	UFUNCTION(Category = "Networked Inventory")
		virtual void AddItemsToInventory(const TArray<FInventoryEntry>& inventoryChanges) override;

	UFUNCTION(Category = "Networked Inventory")
		virtual void RemoveItemsFromInventory(const TArray<FInventoryEntry>& inventoryChanges) override;

	UFUNCTION(BlueprintCallable, Category = "Networked Inventory")
		virtual FString ToString() const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

		
};
