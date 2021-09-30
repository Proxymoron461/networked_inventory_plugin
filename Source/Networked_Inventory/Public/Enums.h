// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.generated.h"

UENUM(BlueprintType)
enum class ESetStatus : uint8
{
	Success UMETA(DisplayName = "Success"),
	CouldNotSetInventory UMETA(DisplayName = "Could Not Set Inventory")
};

UENUM(BlueprintType)
enum class EChangeStatus : uint8
{
	Success UMETA(DisplayName = "Success"),
	CouldNotMakeChange UMETA(DisplayName = "Could Not Make Changes")
};

UENUM(BlueprintType)
enum class EChangeGroupStatus : uint8
{
	AllSuccessful UMETA(DisplayName = "All Successful"),
	SomeChangesLost UMETA(DisplayName = "Some Changes Lost")
};

UENUM(BlueprintType)
enum class ERemovalStatus : uint8
{
	Success UMETA(DisplayName = "Success"),
	ItemNotInInventory UMETA(DisplayName = "Item Not In Inventory")
};

UENUM(BlueprintType)
enum class EAddStatus : uint8
{
	Success UMETA(DisplayName = "Success"),
	ItemAlreadyInInventory UMETA(DisplayName = "Item Already In Inventory")
};

UENUM(BlueprintType)
enum class EPickupReplication : uint8
{
	OnlyOnClient UMETA(DisplayName = "Only On Client"),
	OnlyOnServer UMETA(DisplayName = "Only On Server"),
	OnClientAndServer UMETA(DisplayName = "On Client And Server")
};
