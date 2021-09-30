// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "Structs.h"
#include "Inventory.h"
#include "InventoryInterface.h"
#include "Components/ActorComponent.h"
#include "RPCBasedInventoryComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NETWORKEDINVENTORY_API URPCBasedInventoryComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()

private:
	UPROPERTY()
		UInventory* mInventory;

	UFUNCTION(Client, Reliable, Category = "Networked Inventory")
		void Client_ModifyInventory(const TArray<FInventoryEntry>& inventoryChanges);

	UFUNCTION(Server, Reliable, Category = "Networked Inventory")
		void Server_ConfirmClientModification(EChangeGroupStatus clientStatus);

	UFUNCTION(Client, Reliable, Category = "Networked Inventory")
		void Client_SetInventory(const UInventory* inventory);

	UFUNCTION(Server, Reliable, Category = "Networked Inventory")
		void Server_ConfirmClientSetInventory(ESetStatus clientStatus);

public:
	URPCBasedInventoryComponent();

	UFUNCTION(Category = "Networked Inventory")
		UInventory* GetInventory();

	UFUNCTION(Server, Reliable, Category = "Networked Inventory")
		void Server_ModifyInventory(const TArray<FInventoryEntry>& inventoryChanges);

	UFUNCTION(Server, Reliable, Category = "Networked Inventory")
		void Server_SetClientInventory();

	UFUNCTION(Category = "Networked Inventory")
		virtual void ModifyInventory(const TArray<FInventoryEntry>& inventoryChanges) override;

	UFUNCTION(Category = "Networked Inventory")
		virtual void AddItemsToInventory(const TArray<FInventoryEntry>& inventoryChanges) override;

	UFUNCTION(Category = "Networked Inventory")
		virtual void RemoveItemsFromInventory(const TArray<FInventoryEntry>& inventoryChanges) override;

	UFUNCTION(BlueprintCallable, Category = "Networked Inventory")
		virtual FString ToString() const override;

protected:
	virtual void BeginPlay() override;
};
