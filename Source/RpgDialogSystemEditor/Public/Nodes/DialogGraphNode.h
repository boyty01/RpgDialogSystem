// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "Nodes/DialogEdGraphNodeBase.h"
#include "DialogTypes.h"
#include "Interface//RpgDialogNodeInterface.h"
#include "Nodes/DialogConditionsNode.h"
#include "DialogGraphNode.generated.h"

/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogGraphNode : public UDialogEdGraphNodeBase 
{
	GENERATED_BODY()
	
public:

	UDialogGraphNode();


	virtual void AddNode(UDialogEdGraphNodeBase* Node) override
	{
		ChildNodes.Add(Node);
		Node->OnNodeDeletedDelegate.AddUObject(this, &UDialogGraphNode::OnSubNodeDeleted);
	}

	virtual FPinConnectionResponse CanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const override;


	virtual bool SupportsConditionsSubNode() const override { return true; };
	virtual bool SupportsEventsSubNode() const override { return true; };

	//REQUIRED. Level Sequence to play. 
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<ULevelSequence> LevelSequenceToPlay;

	//Is this dialog considered an entry point to a conversation
	UPROPERTY(EditAnywhere)
	bool bEntryPoint;

	// If flagged as an entry point, this value will determine tie breakers with other qualified entry points. Higher priority wins.
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bEntryPoint"))
	int StartPriority;

	/* Split subtitles that support timing via Level Sequence Markers. Markers named "SUB" will trigger the next subtitle in the array, if it exists. */
	UPROPERTY(EditAnywhere, meta = (MultiLine = "true", TextAreaHeight = "5"))
	TArray<FText> Subtitles;

	// Can the player skip this dialog sequence?
	UPROPERTY(EditAnywhere)
	bool bSkippable{ true };


	static const FName InputPin_Event_Name;
	static const FName InputPin_InDialog_Name;
	static const FName InputPin_InPlayerChoice_Name;
	static const FName InputPin_SelectionCondition_Name;
	static const FName OutputPin_OutDialog_Name;
	static const FName OutputPin_OutPlayerChoice_Name;


	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return NodeTitle.IsEmpty() ? FText::FromString("Dialog Node") : NodeTitle;
	}

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);

		if (UEdGraph* Graph = GetGraph())
		{
			Graph->NotifyGraphChanged();
		}
	}

	virtual void ValidateNodeConfiguration() override;

	virtual void ValidateSubNodes() override;

	virtual EDialogGraphConfigurationState GetHighestErrorStateInSubNodes();

	virtual void NodeConnectionListChanged() override;
#endif

private: 
	virtual FPinConnectionResponse Internal_NextDialogCanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const;
	virtual FPinConnectionResponse Internal_PlayerChoicesCanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const;
};
