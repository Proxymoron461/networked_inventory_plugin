// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs.generated.h"

USTRUCT()
struct FInventoryEntry
{
	GENERATED_BODY()

	FInventoryEntry() : ItemCode(), Quantity(0) {}
	FInventoryEntry(FName code, int32 quantity) : ItemCode(code), Quantity(quantity) {}

	UPROPERTY()
		FName ItemCode;

	UPROPERTY()
		int32 Quantity;

	//check to see if the aggro record matches another aggro record by overloading the "==" operator.
	FORCEINLINE bool operator==(const FInventoryEntry& Other) const
	{
		return ItemCode == Other.ItemCode && Quantity == Other.Quantity;
	}
};
