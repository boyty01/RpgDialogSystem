// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DialogTypes.h"
#include "DialogTree.generated.h"




/**
 * 
 */
UCLASS(BlueprintType)
class RPGDIALOGSYSTEM_API UDialogTree : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Sequences")
	TMap<FGuid, FNpcDialogSequence> Sequences;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Choices")
	TMap<FGuid, FDialogChoice> DialogChoices;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Conditions")
	TMap<FGuid, FDialogCondition> Conditions;

	UPROPERTY(VisibleAnywhere, Category = "Events")
	TMap<FGuid, FDialogEventData> Events;

	//List of Base dialog states for this tree. 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialog States")
	TMap<FName, bool> DialogStates;

	

	/* EdGraph support. EDITOR ONLY */
#if WITH_EDITORONLY_DATA

	// Holds node layout info for graph rebuilding
	UPROPERTY()
	TMap<FGuid, FDialogGraphNodeData> EditorNodeLayout;

	UPROPERTY()
	UEdGraph* EditorGraph;

#endif


};
