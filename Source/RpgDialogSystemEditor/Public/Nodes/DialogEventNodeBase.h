// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/DialogEdGraphNodeBase.h"
#include "DialogTypes.h"
#include "DialogEventNodeBase.generated.h"

/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogEventNodeBase : public UDialogEdGraphNodeBase
{
	GENERATED_BODY()
public:

	UDialogEventNodeBase()
	{
		bIsSubNode = true;
	}

	inline static const FName OutputPin_Event_Name = FName("OutEvent");

	UPROPERTY()
	EDialogSystemTarget TargetSystem;

	UPROPERTY()
	EDialogActionType ActionType;

	// ID of the condition type (i.e. Quest/Item ID)
	UPROPERTY(EditAnywhere)
	FName Id;

	UPROPERTY()
	bool bCanEditValue{ false };

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCanEditValue", ClampMin = "0"))
	int Value{ 1 };

	virtual FString GetSerializedString() const { return FString(); };

	virtual void AllocateDefaultPins() override;
	virtual FPinConnectionResponse CanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return NodeTitle.IsEmpty() ? FText::FromString("Uninitialized Event Node") : NodeTitle;
	}
	virtual void ValidateNodeConfiguration() override;

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void UpdateNodeTitle() {};

	//Best place to do any additional specific setup on a subclassed node. Default used to invert the action type from the global enum to the UI friendly ones.
	virtual void UpdateEventNodePostInit() {};

	virtual FSlateColor GetTitleBackgroundColor() const { return FLinearColor(0.7f, 0.7f, 0.7f, 0.7f); };
};
