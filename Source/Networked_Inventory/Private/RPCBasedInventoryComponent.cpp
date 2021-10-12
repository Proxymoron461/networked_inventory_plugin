// Fill out your copyright notice in the Description page of Project Settings.


#include "RPCBasedInventoryComponent.h"

// Sets default values for this component's properties
URPCBasedInventoryComponent::URPCBasedInventoryComponent()
{
	// No need to tick.
	PrimaryComponentTick.bCanEverTick = false;

	Inventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));
}

// Called when the game starts
void URPCBasedInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void URPCBasedInventoryComponent::Server_ConfirmClientModification_Implementation(EChangeGroupStatus serverStatus)
{
	ensure(GetOwner()->GetLocalRole() == ROLE_Authority);

	UE_LOG(LogTemp, Log, TEXT("Server confirming client changes..."));
	if (serverStatus != EChangeGroupStatus::AllSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("Client could not match changes; needs correction. Client status: %s"), *UEnum::GetValueAsString(serverStatus));
		UE_LOG(LogTemp, Log, TEXT("Setting client to server inventory..."));
		Server_SetClientInventory();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Server changes confirmed."));
}

void URPCBasedInventoryComponent::Client_ModifyInventory_Implementation(const TArray<FInventoryEntry>& inventoryChanges)
{
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Log, TEXT("Cannot run inventory modifications intended for client on server. Cancelling..."));
		return;
	}

	checkCode(
		for (const FInventoryEntry& entry : inventoryChanges)
		{
			if (entry.Quantity == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Adding zero quantity of item %s."));
			}
		}
	);

	TTuple<EChangeGroupStatus, TArray<EChangeStatus>> pair = Inventory->ModifyGroupOfEntries(inventoryChanges);

	Server_ConfirmClientModification(pair.Key);
}

void URPCBasedInventoryComponent::Server_ModifyInventory_Implementation(const TArray<FInventoryEntry>& inventoryChanges)
{
	ensure(GetOwner()->GetLocalRole() == ROLE_Authority);
	checkCode(
		for (const FInventoryEntry& entry : inventoryChanges)
		{
			if (entry.Quantity == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Adding zero quantity of item %s."));
			}
		}
	);

	TTuple<EChangeGroupStatus, TArray<EChangeStatus>> pair = Inventory->ModifyGroupOfEntries(inventoryChanges);

	if (pair.Key != EChangeGroupStatus::AllSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not all inventory changes successful. Some lost."));
	}

	Client_ModifyInventory(inventoryChanges);
}

UInventory* URPCBasedInventoryComponent::GetInventory()
{
	return Inventory;
}

FString URPCBasedInventoryComponent::ToString() const
{
	return Inventory->ToString();
}

int32 URPCBasedInventoryComponent::Num() const
{
	return Inventory->Num();
}

bool URPCBasedInventoryComponent::Contains(const FName itemCode) const
{
	return Inventory->Contains(itemCode);
}

void URPCBasedInventoryComponent::ModifyInventory(const TArray<FInventoryEntry>& inventoryChanges)
{
	Server_ModifyInventory(inventoryChanges);
}

void URPCBasedInventoryComponent::AddItemsToInventory(const TArray<FInventoryEntry>& inventoryChanges)
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
	Server_ModifyInventory(inventoryChanges);
}

void URPCBasedInventoryComponent::RemoveItemsFromInventory(const TArray<FInventoryEntry>& inventoryChanges)
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

	Server_ModifyInventory(finalChanges);
}

void URPCBasedInventoryComponent::Server_SetClientInventory_Implementation()
{
	ensure(GetOwner()->GetLocalRole() == ROLE_Authority);
	Client_SetInventory(GetInventory());
}

void URPCBasedInventoryComponent::Client_SetInventory_Implementation(const UInventory* inventoryPtr)
{
	ensure(GetOwner()->GetLocalRole() != ROLE_Authority);
	Inventory = DuplicateObject<UInventory>(inventoryPtr, this, TEXT("Inventory"));
	Server_ConfirmClientSetInventory(ESetStatus::Success);
}

void URPCBasedInventoryComponent::Server_ConfirmClientSetInventory_Implementation(ESetStatus serverStatus)
{
	ensure(GetOwner()->GetLocalRole() == ROLE_Authority);
	if (serverStatus != ESetStatus::Success)
	{
		UE_LOG(LogTemp, Error, TEXT("Client could not set inventory; needs correction. Client status: %s"), *UEnum::GetValueAsString<ESetStatus>(serverStatus));
		UE_LOG(LogTemp, Log, TEXT("Attempting to set client to server inventory..."));
		Server_SetClientInventory();
	}
}
