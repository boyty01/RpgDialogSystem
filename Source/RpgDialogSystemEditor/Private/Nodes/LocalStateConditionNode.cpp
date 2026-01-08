// Copyright DMTesseracT LTd. All rights reserved.


#include "Nodes/LocalStateConditionNode.h"
#include "Graph/DialogGraph.h"
#include "DataAsset/DialogTree.h"
void ULocalStateConditionNode::ValidateNodeConfiguration()
{
	if (Id.IsNone())
	{
		ConfigurationState = EDialogGraphConfigurationState::Warning;
		ErrorMsg = TEXT("State Condition Node has no Key specified.");
		return;
	}

	if (UDialogGraph* DialogGraph = Cast<UDialogGraph>(GetGraph()))
	{
		if (UDialogTree* Tree = Cast<UDialogTree>(DialogGraph->GetOuter()))
		{
			if(!Tree->DialogStates.Contains(Id))
			{ConfigurationState = EDialogGraphConfigurationState::Error;
				ErrorMsg = TEXT("State Condition Node references a State Key that does not exist in the Dialog Tree.");
				return;
			}
		}
	}
	
	ConfigurationState = EDialogGraphConfigurationState::Valid;
	ErrorMsg = "";
}

FString ULocalStateConditionNode::GetSerializedString() const
{
	return FString();
}

void ULocalStateConditionNode::UpdateNodeTitle()
{
	NodeTitle = FText::Format(
		FText::FromString("State: {0} Must Be {1}"), 
		{ 
			FText::FromString(Id.ToString()), 
			bNegate ?  FText::FromString("False") : FText::FromString("True") 
		}
	);
}
