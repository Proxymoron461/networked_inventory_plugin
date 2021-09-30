// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InventoryItem.generated.h"

UCLASS()
class NETWORKEDINVENTORY_API AInventoryItem : public AActor
{
	GENERATED_BODY()

private:

	UFUNCTION(Server, Reliable, Category = "Inventory Item")
		void Server_Destroy();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Item", meta = (AllowPrivateAccess = true))
		FName mItemCode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Item", meta = (AllowPrivateAccess = true))
		int32 mQuantity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Item", meta = (AllowPrivateAccess = true))
		EPickupReplication mReplicationSetting;
	
public:	
	// Sets default values for this actor's properties
	AInventoryItem();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Item")
		void Pickup(AActor* actorPickingUp);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	

};
