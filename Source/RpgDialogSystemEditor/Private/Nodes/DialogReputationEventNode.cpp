// Copyright DMTesseracT LTd. All rights reserved.


#include "Nodes/DialogReputationEventNode.h"

UDialogReputationEventNode::UDialogReputationEventNode()
{
	TargetSystem = EDialogSystemTarget::REPUTATION;
	NodeTitle = FText::FromString("Reputation Event Node");
}

void UDialogReputationEventNode::UpdateNodeTitle()
{
	// DevNote: this isn't particularly friendly looking or scalable!
	FString ActionText = "Modify";

	NodeTitle = FText::FromString(FString::Format(TEXT("{0} {1}'s Reputation: {2}"), { ActionText, Id.ToString(), Value}));

}

void UDialogReputationEventNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{

}

void UDialogReputationEventNode::UpdateEventNodePostInit()
{	
	ReputationAction = EDialogReputationEventAction::MODIFY_REPUTATION;
}

FSlateColor UDialogReputationEventNode::GetTitleBackgroundColor() const
{
	return FLinearColor(0.7f, 0.7f, 0.7f, 0.7f);
}

void UDialogReputationEventNode::ValidateNodeConfiguration()
{
	if (Id.IsNone())
	{
		ConfigurationState = EDialogGraphConfigurationState::Warning;
		ErrorMsg = TEXT("Reputation Event Node has no Reputation ID specified.");
		return;
	}

	ConfigurationState = EDialogGraphConfigurationState::Valid;
	ErrorMsg = "";

}
