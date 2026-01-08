// Copyright DMTesseracT LTd. All rights reserved.


#include "Nodes/DialogCustomEventNode.h"

UDialogCustomEventNode::UDialogCustomEventNode()
{
	TargetSystem = EDialogSystemTarget::SCRIPT;
	ActionType = EDialogActionType::CUSTOM_SCRIPT;
	NodeTitle = FText::FromString("Custom Script Event Node");
}

void UDialogCustomEventNode::UpdateNodeTitle()
{
	NodeTitle = FText::FromString(FString::Format(TEXT("Custom Script: {0}"), { CustomEventScriptClass ? CustomEventScriptClass->GetName() : "None" }));
}

FSlateColor UDialogCustomEventNode::GetTitleBackgroundColor() const
{
	return FLinearColor(0.7f, 0.7f, 0.7f, 0.7f);
}

void UDialogCustomEventNode::ValidateNodeConfiguration()
{
	if (!CustomEventScriptClass)
	{
		ConfigurationState = EDialogGraphConfigurationState::Error;
		ErrorMsg = TEXT("Custom Event Node has an invalid 'Custom Event Script Class' specified.");
		return;
	}
	ConfigurationState = EDialogGraphConfigurationState::Valid;
	ErrorMsg = "";
}
