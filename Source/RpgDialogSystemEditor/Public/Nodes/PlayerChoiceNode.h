// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/DialogGraphNode.h"
#include "EdGraph/EdGraphSchema.h"
#include "Interface/RpgDialogNodeInterface.h"
#include "PlayerChoiceNode.generated.h"

/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UPlayerChoiceNode : public  UDialogEdGraphNodeBase
{
	GENERATED_BODY()

public:
	virtual bool CanUserDeleteNode() const override { return true; };

	static const FName InputPin_InDialog_Name;
	static const FName OutputPin_OutDialog_Name;
	static const FName InputPin_Events_Name; 
	static const FName InputPin_SelectionConditions_Name;
	
	virtual void AddNode(UDialogEdGraphNodeBase* Node) override
	{
		ChildNodes.Add(Node);
		Node->OnNodeDeletedDelegate.AddUObject(this, &UDialogGraphNode::OnSubNodeDeleted);
	}

	bool GetShowAttributesOnNode() const override
	{
		return true;
	}

	UPROPERTY(EditAnywhere, Category ="Player Response")
	FText ResponseText;

	UPROPERTY()
	FDialogGraphNodeData NodeData;

	virtual FPinConnectionResponse CanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const override;
	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override 
	{
		return NodeTitle.IsEmpty() ? FText::FromString("Choice Node") : NodeTitle;
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

	virtual bool SupportsConditionsSubNode() const { return true; };

	virtual bool SupportsEventsSubNode() const { return true; };

	void NodeConnectionListChanged() override;

	virtual void ValidateNodeConfiguration() override;
	virtual void ValidateSubNodes() override;

	EDialogGraphConfigurationState GetHighestErrorStateInSubNodes();
#endif
};
