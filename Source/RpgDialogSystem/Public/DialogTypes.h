// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DialogTypes.generated.h"

class ULevelSequence;
class UAnimationAsset;
class UDialogScriptEventBase;

UENUM()
enum class EQuestState : uint32
{
	NOT_STARTED,
	IN_PROGRESS,
	COMPLETED
};

UENUM()
enum class EDialogActionType : uint8
{
	ADD_QUEST,
	REMOVE_QUEST,
	PROGRESS_QUEST,
	COMPLETE_QUEST,
	FAIL_QUEST,
	ADD_ITEM,
	REMOVE_ITEM,
	SET_STATE,
	CUSTOM_SCRIPT
};


UENUM()
enum class EDialogSystemTarget : uint8
{
	QUEST,
	INVENTORY,
	STATE,
	SCRIPT
};


UENUM()
enum class EDialogConditionRequirementType : uint8
{
	HAVE,
	IS_COMPLETE,
	HAS_COMPLETED,
	HAS_FAILED
};


static class FDialogConditionString
{
public:
	inline static const FString HasQuest = "HasQuest";
	inline static const FString CompletedQuest = "CompletedQuest";
	inline static const FString HasItem = "HasItem";
	inline static const FString HasNumItems = "HasNumItem";
	inline static const FString QuestReadyToTurnIn = "IsReadyToTurnIn";
	inline static const FString FailedQuest = "FailedQuest";
};


/* Dialog Condition data. */
USTRUCT(BlueprintType)
struct FDialogCondition
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid ConditionId;

	UPROPERTY()
	EDialogConditionRequirementType ConditionType;

	UPROPERTY()
	EDialogSystemTarget TargetSystem;

	//The FName of the ID relative to the target system. i.e. Data row name for quest table etc.
	UPROPERTY()
	FName DataId;

	//Whether to negate the Condition Type i.e. HAVE becomes NOT_HAVE.
	UPROPERTY()
	bool bNegate;

	// editable int value for additional info. typically for N of Item (inventory) requirements.
	UPROPERTY(meta = (ClampMin = "1"))
	int Value{ 1 };
};

USTRUCT(BlueprintType)
struct FDialogEventData
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid DialogEventId;

	UPROPERTY()
	EDialogSystemTarget TargetSystem;

	UPROPERTY()
	EDialogActionType EventAction;

	UPROPERTY()
	TSoftClassPtr<UDialogScriptEventBase> ScriptClass;

	UPROPERTY()
	FName DataId;

	UPROPERTY()
	int Value;
};


USTRUCT(BlueprintType)
struct FDialogChoice
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid DialogChoiceId;

	// the text to display as the dialog choice.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (WrapText = "true"))
	FText DisplayText;

	// Conditions for each choice presented to the player at the end of the sequence. (e.g., "HasQuest:1")
	UPROPERTY(VisibleAnywhere)
	TArray<FGuid> QualifyingConditions;

	UPROPERTY(VisibleAnywhere)
	FGuid NextSequenceId;

	UPROPERTY(VisibleAnywhere)
	TArray<FGuid> TriggeredEvents;


	FDialogChoice() {};
};


USTRUCT(BlueprintType)
struct FNpcDialogSequence
{
	GENERATED_BODY()

	// unique ID for synchronising graph node data with associated sequence data.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Editor")
	FGuid SequenceId{ FGuid() };

	/* If true, this sequence is considered viable as an initial dialog sequence, and it's QualifyingConditions will be assessed
	when the dialog interaction is initiated. if qualified, it will be selected to play if its priority is highest of all qualified sequences.
	Sequences that are specifically triggered by dialog options should set this to false.
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Qualification")
	bool bConsideredForDialogStart;

	// Priority that this sequence has when being considered for the initial dialog. Highest priority will be selected.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Qualification", meta = (EditCondition = "bConsideredForDialogStart"))
	int SelectionPriority{ 1 };

	/* list of dialog conditions that must be met for this dialog to be allowed to play. Usually only relevant
	 to sequences that are not initiated from previous dialog options (such as starting a new dialog interaction).
	 Default implementations do not care about these conditions when a sequence is selected from a player dialog choice,
	 since their viability should already be evaluated from the ChoiceConditions.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Qualification", meta = (EditCondition = "bConsideredForDialogStart"))
	TArray<FGuid> QualifyingConditions;

	/*Level sequence to instantiate and play. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LevelSequence")
	TSoftObjectPtr<ULevelSequence> LevelSequence;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialog")
	TArray<FText> Subtitles;

	// Multiple choice options for the player, presented at the end of the sequence
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Options")
	TArray<FGuid> DialogChoices;

	// if a valid ID, will go to this sequence immediately after this sequence ends, bypassing any dialog choices.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Settings")
	FGuid AutoSelectNextSequence{FGuid()};

	//Inventory and quest events triggered when the sequence starts
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Triggered Events")
	TArray<FGuid> TriggeredEvents;

	//When this sequence is played, the owning NPC will execute the script specified below when the dialog ends. This will be overridden if subsequent sequences play and specify a script.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Triggered Events")
	TSoftClassPtr<UDialogScriptEventBase> DialogEndScriptEvent;

	/* Whether this sequence can be skipped by user. Default true. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	bool bSkippable{ true };

	FNpcDialogSequence() {};


	bool operator==(FGuid OtherId) const
	{
		return SequenceId == OtherId;
	}
};



USTRUCT()
struct FDialogGraphNodeConnection
{
	GENERATED_BODY()

	UPROPERTY()
	FName OutputPinName;

	UPROPERTY()
	FGuid TargetNodeID;

	UPROPERTY()
	FName TargetInputPin;

	FDialogGraphNodeConnection() {};

	FDialogGraphNodeConnection(FName _OutPin, FGuid _TargetNodeId, FName _TargetInPin)
	{
		OutputPinName = _OutPin;
		TargetNodeID = _TargetNodeId;
		TargetInputPin = _TargetInPin;
	};

	bool operator==(const FDialogGraphNodeConnection& Other) const
	{
		return TargetNodeID == Other.TargetNodeID && TargetInputPin == Other.TargetInputPin;
	}
};

USTRUCT()
struct FDialogGraphNodeData
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<UEdGraphNode> NodeClass;

	// Unique ID to associate this with editor node instances
	UPROPERTY()
	FGuid NodeID;

	// Position in the graph editor
	UPROPERTY()
	FVector2D NodePosition;

	// graph title for the node.
	UPROPERTY()
	FText NodeTitle;

	// if true, the node is in "expanded" mode if supported.
	UPROPERTY()
	bool bShowAttributesOnNode;

	// Array of output pin connections
	UPROPERTY()
	TArray<FDialogGraphNodeConnection> OutputPinConnections;

	UPROPERTY()
	FSlateColor Colour;

	bool operator==(FGuid OtherId) const
	{
		return NodeID == OtherId;
	}

	FDialogGraphNodeData() 
	{
		NodeID = FGuid();
	}

	FDialogGraphNodeData(
		TSubclassOf<UEdGraphNode> _NodeClass,
		FGuid _NodeId,
		FVector2D _Position,
		FText _Title,
		bool _bShowAttributes,
		TArray<FDialogGraphNodeConnection> _OutPinConnections
	)
	{
		NodeClass = _NodeClass;
		NodeID = _NodeId;
		NodePosition = _Position;
		NodeTitle = _Title;
		bShowAttributesOnNode = _bShowAttributes;
		OutputPinConnections = _OutPinConnections;
	};
};
