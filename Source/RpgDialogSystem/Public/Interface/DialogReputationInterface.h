// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DialogReputationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDialogReputationInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGDIALOGSYSTEM_API IDialogReputationInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Reputation")
	void ModifyReputation(const FName ReputationType, const int32 Amount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Reputation")
	int32 GetReputation(const FName ReputationType) const;
};
