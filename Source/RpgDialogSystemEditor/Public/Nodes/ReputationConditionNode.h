// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/DialogConditionsNode.h"
#include "DialogTypes.h"
#include "ReputationConditionNode.generated.h"



/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UReputationConditionNode : public UDialogConditionsNode
{
	GENERATED_BODY()
	
public:

	UReputationConditionNode()
	{
		bCanEditNumValue = false;
		bCanEditAction = true;
		ConditionTarget = EDialogSystemTarget::REPUTATION;
		UpdateNodeTitle();
	}

	virtual void ValidateNodeConfiguration() override;
	virtual FString GetSerializedString() const override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return NodeTitle.IsEmpty() ? FText::FromString("Reputation Condition: None") : NodeTitle;
	}

	void UpdateNodeTitle() override;
};
