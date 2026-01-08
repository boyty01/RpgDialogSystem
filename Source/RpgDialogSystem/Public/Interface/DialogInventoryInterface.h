// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DialogInventoryInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDialogInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGDIALOGSYSTEM_API IDialogInventoryInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:


	// Check if the player has a specific item by ID.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	bool HasItem(const FName ItemId);

	// Check if the player has a specific number of items by ID.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	bool HasNumItem(const FName ItemId, const int Num);

	// add N items to the players inventory
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	bool AddItem(const FName ItemId, const int NumItems);

	// remove N items from the players inventory
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	bool RemoveItem(const FName ItemId, const int NumItems);

};
