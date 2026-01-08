// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/DialogConditionsNode.h"
#include "InventoryConditionNode.generated.h"


/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UInventoryConditionNode : public UDialogConditionsNode
{
	GENERATED_BODY()
	
public:

	UInventoryConditionNode()
	{
		bCanEditNumValue = true;
		bCanEditAction = false;
		RequirementType = EDialogConditionRequirementType::HAVE;
		ConditionTarget = EDialogSystemTarget::INVENTORY;
	}

	virtual FString GetSerializedString() const override;

	virtual void ValidateNodeConfiguration() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return NodeTitle.IsEmpty() ? FText::FromString("State Condition: ") : NodeTitle;
	}

	virtual void UpdateNodeTitle() override;
};
