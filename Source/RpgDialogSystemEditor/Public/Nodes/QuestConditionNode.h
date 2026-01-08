// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/DialogConditionsNode.h"
#include "DialogTypes.h"
#include "QuestConditionNode.generated.h"



/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UQuestConditionNode : public UDialogConditionsNode
{
	GENERATED_BODY()
	
public:

	UQuestConditionNode()
	{
		bCanEditNumValue = false;
		bCanEditAction = true;
		ConditionTarget = EDialogSystemTarget::QUEST;
		UpdateNodeTitle();
	}

	virtual void ValidateNodeConfiguration() override;
	virtual FString GetSerializedString() const override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return NodeTitle.IsEmpty() ? FText::FromString("Quest Condition: None") : NodeTitle;
	}

	void UpdateNodeTitle() override;
};
