// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "Interface/RpgDialogNodeInterface.h"
#include "DialogEventNode_Base.generated.h"


UENUM()
enum class ESequenceEventCategory : uint8
{
	QUEST,
	INVENTORY
};

UENUM()
enum class ESequenceEventAction :  uint8
{
	ADD,
	REMOVE,
	PROGRESS
};

/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogEventNode_Base : public UEdGraphNode, public IRpgDialogNodeInterface
{
	GENERATED_BODY()

public:
	virtual bool CanUserDeleteNode() const override { return true; };

	UPROPERTY()
	ESequenceEventCategory EventCategoryType;

	UPROPERTY()
	ESequenceEventAction EventActionType;

	// The property ID of the target action. e.g. quest id or item id.
	UPROPERTY(EditAnywhere, Category = "Dialog Event")
	FName ActionTargetId;

	virtual FPinConnectionResponse CanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const override;
	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override 
	{
		return FText::FromString("Untitled Sequence Event");
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
#endif
};
