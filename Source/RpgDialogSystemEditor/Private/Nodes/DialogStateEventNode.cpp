// Copyright DMTesseracT LTd. All rights reserved.


#include "Nodes/DialogStateEventNode.h"
#include "DataAsset/DialogTree.h"

UDialogStateEventNode::UDialogStateEventNode()
{
	TargetSystem = EDialogSystemTarget::STATE;
	ActionType = EDialogActionType::SET_STATE;
	bCanEditValue = true;
	NodeTitle = FText::FromString("State Event");
}

void UDialogStateEventNode::UpdateNodeTitle()
{
	FString NewValue = Value ? TEXT("True") : TEXT("False");
	NodeTitle = FText::FromString(FString::Format(TEXT("Set State: \'{0}\' to {1}"), { Id.ToString(), NewValue }));
}

void UDialogStateEventNode::UpdateEventNodePostInit()
{
	
}

void UDialogStateEventNode::ValidateNodeConfiguration()
{
	Super::ValidateNodeConfiguration();

	if (UDialogTree* Tree = Cast<UDialogTree>(GetGraph()->GetOuter()))
	{
		if (!Tree->DialogStates.Contains(Id))
		{
			if (ErrorMsg.Len() > 0)
			{
				ErrorMsg += TEXT("\n");
			}
			ConfigurationState = EDialogGraphConfigurationState::Error;
			ErrorMsg += TEXT("- Dialog State Event Node references a Dialog State that does not exist in the Dialog Tree.");
			return;
		}
	}
}
