#include "Nodes/DialogEventNode_AddQuest.h"

UDialogEventNode_AddQuest::UDialogEventNode_AddQuest()
{
	EventCategoryType = ESequenceEventCategory::QUEST;
	EventActionType = ESequenceEventAction::ADD;
}
