// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DialogQuestInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDialogQuestInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGDIALOGSYSTEM_API IDialogQuestInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Notify that a quest dialog option was selected.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	bool QuestDialogSelected(FName EventId);

	// Add a quest to the player's quest log by ID.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	bool AddQuest(const FName QuestId);

	//Remove a quest from the player's quest log by ID.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	bool RemoveQuest(const FName QuestId);

	// Complete a quest by ID.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	bool CompleteQuest(const FName QuestId);

	// Fail a quest with the specific id.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	bool FailQuest(const FName QuestId);

	// Check if the player has a specific quest by ID.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	bool HasQuest(const FName QuestId);

	// Check if the player has completed a specific quest by ID. This should not check if the quest is active, only if it is in the completed quests list.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	bool CompletedQuest(const FName QuestId);

	// Check if the player has a specific active quest by ID, and that all objectives are complete.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	bool IsQuestReadyForTurnIn(const FName QuestId);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Dialog System")
	bool HasFailedQuest(const FName QuestId);
};
