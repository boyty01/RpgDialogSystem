// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/DialogConditionsNode.h"
#include "LocalStateConditionNode.generated.h"

/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API ULocalStateConditionNode : public UDialogConditionsNode
{
	GENERATED_BODY()

	ULocalStateConditionNode()
	{
		bCanEditNumValue = false;
		bCanEditAction = false;
		ConditionTarget = EDialogSystemTarget::STATE;
	}
public:

	virtual void ValidateNodeConfiguration() override;

	virtual FString GetSerializedString() const override;
	
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return NodeTitle.IsEmpty() ? FText::FromString("") : NodeTitle;
	}

	virtual void UpdateNodeTitle() override;

};
