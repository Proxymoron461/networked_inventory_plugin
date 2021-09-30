// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItem.h"
#include "InventoryInterface.h"
#include "Inventory.h"

// Sets default values
AInventoryItem::AInventoryItem()
{

	bReplicates = true;  // To ensure that when server copy is destroyed, the client ones are too

	mItemCode = FName(TEXT("INVALID"));
	mQuantity = 1;
	mReplicationSetting = EPickupReplication::OnClientAndServer;
}

void AInventoryItem::Pickup_Implementation(AActor* actorPickingUp)
{
	TArray<UActorComponent*> components = actorPickingUp->GetComponentsByInterface(UInventoryInterface::StaticClass());
	if (components.Num() > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("More than one inventory component found. Number found: %i"), components.Num());
		return;
	}

	if (components.Num() < 1)
	{
		UE_LOG(LogTemp, Log, TEXT("No inventory component found. Cancelling pickup..."));
		return;
	}

	if (mReplicationSetting == EPickupReplication::OnlyOnClient && actorPickingUp->GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Log, TEXT("Replication setting disallows running pickup events on server. Cancelling..."));
		return;
	}

	if (mReplicationSetting == EPickupReplication::OnlyOnServer && actorPickingUp->GetLocalRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Log, TEXT("Replication setting disallows running pickup events on client. Cancelling..."));
		return;
	}

	IInventoryInterface* inventory = Cast<IInventoryInterface>(components[0]);
	if (inventory)
	{
		UE_LOG(LogTemp, Log, TEXT("Picking up item..."));
		inventory->AddItemToInventory(mItemCode, mQuantity);
		UE_LOG(LogTemp, Log, TEXT("Destroying..."));
		Destroy();  // Destroy local copy
		Server_Destroy();  // Ensure replicated one is destroyed too
	}
}

void AInventoryItem::Server_Destroy_Implementation()
{
	Destroy();  // Should replicate to all client copies!
}

// Called when the game starts or when spawned
void AInventoryItem::BeginPlay()
{
	Super::BeginPlay();

}

