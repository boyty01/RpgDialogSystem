// Copyright DMTesseracT LTd. All rights reserved.


#include "Nodes/DialogQuestEventNode.h"

UDialogQuestEventNode::UDialogQuestEventNode()
{
	TargetSystem = EDialogSystemTarget::QUEST;
	NodeTitle = FText::FromString("Quest Event Node");
}

void UDialogQuestEventNode::UpdateNodeTitle()
{
	// DevNote: this isn't particularly friendly looking or scalable!
	FString ActionText = QuestAction == EDialogQuestEventAction::GIVE_QUEST ? "Give" : 
		QuestAction == EDialogQuestEventAction::REMOVE_QUEST ? "Remove" : 
		QuestAction == EDialogQuestEventAction::PROGRESS_QUEST ?  "Progress" :
		QuestAction == EDialogQuestEventAction::FAIL_QUEST ? "Fail" : 
		"Complete";

	NodeTitle = FText::FromString(FString::Format(TEXT("{0} Quest: {1}"), { ActionText, Id.ToString()}));

}

void UDialogQuestEventNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	switch (QuestAction)
	{
	case EDialogQuestEventAction::GIVE_QUEST:
		ActionType = EDialogActionType::ADD_QUEST;
		break;
	case EDialogQuestEventAction::REMOVE_QUEST:
		ActionType = EDialogActionType::REMOVE_QUEST;
		break;
	case EDialogQuestEventAction::COMPLETE_QUEST:
		ActionType = EDialogActionType::COMPLETE_QUEST;
		break;
	case EDialogQuestEventAction::PROGRESS_QUEST:
		ActionType = EDialogActionType::PROGRESS_QUEST;
		break;
	case EDialogQuestEventAction::FAIL_QUEST:
		ActionType = EDialogActionType::FAIL_QUEST;
		break;
	default:
		ActionType = EDialogActionType::ADD_QUEST;
		break;
	}
}

void UDialogQuestEventNode::UpdateEventNodePostInit()
{	
	QuestAction = ActionType == EDialogActionType::ADD_QUEST ? EDialogQuestEventAction::GIVE_QUEST :
		ActionType == EDialogActionType::COMPLETE_QUEST ? EDialogQuestEventAction::COMPLETE_QUEST : 
		ActionType == EDialogActionType::PROGRESS_QUEST ? EDialogQuestEventAction::PROGRESS_QUEST :
		ActionType == EDialogActionType::FAIL_QUEST ? EDialogQuestEventAction::FAIL_QUEST :
		EDialogQuestEventAction::REMOVE_QUEST;
}

FSlateColor UDialogQuestEventNode::GetTitleBackgroundColor() const
{
	return FLinearColor(0.7f, 0.7f, 0.7f, 0.7f);
}

void UDialogQuestEventNode::ValidateNodeConfiguration()
{
	if (Id.IsNone())
	{
		ConfigurationState = EDialogGraphConfigurationState::Warning;
		ErrorMsg = TEXT("Quest Event Node has no Quest ID specified.");
		return;
	}

	ConfigurationState = EDialogGraphConfigurationState::Valid;
	ErrorMsg = "";

}
