// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DialogInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDialogInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface to be implemented by relevant pawns that will interact with the dialog system.
 * This allows the dialog system to automatically notify the player pawn and appropriate NPC of dialog events, such as the start and end of dialog sequences, without having to know the specifics of the pawn class,
 * or having to bind the delegates manually in each pawn class.
 */

class RPGDIALOGSYSTEM_API IDialogInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:


	// Invoked on player pawn and relevant Dialog NPC when a new dialog sequence begins.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	void DialogSequenceInitalised(struct FNpcDialogSequence sequence);

	// Invoked on player pawn and relevant Dialog NPC at the end of a level sequence.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	void DialogSequenceEnded(struct FNpcDialogSequence sequence);

	// Invoked on player pawn and relevant Dialog NPC when dialog tree conversation initialises.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	void BeginDialogTreeInteraction();

	// Invoked on player pawn and relevant Dialog NPC when dialog tree conversation ends.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	void EndDialogTreeInteraction();

	// Event invoked on player pawn and relevant Dialog NPC if dialog tree fails to play for any reason.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	void OnFailedToPlayDialogTree(const FString& Reason);



};
