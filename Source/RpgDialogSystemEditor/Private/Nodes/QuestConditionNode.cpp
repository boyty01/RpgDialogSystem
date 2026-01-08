// Copyright DMTesseracT LTd. All rights reserved.


#include "Nodes/QuestConditionNode.h"


void UQuestConditionNode::ValidateNodeConfiguration()
{
    if (Id.IsNone())
    {
        ConfigurationState = EDialogGraphConfigurationState::Warning;
        ErrorMsg = TEXT("Quest Condition Node has no Quest ID specified.");
    }
    else
    {
        ConfigurationState = EDialogGraphConfigurationState::Valid;
        ErrorMsg = "";
	}
}

FString UQuestConditionNode::GetSerializedString() const
{
    FString SerializedCondition;
    SerializedCondition = RequirementType == EDialogConditionRequirementType::HAVE ? FDialogConditionString::HasQuest :
        RequirementType == EDialogConditionRequirementType::HAS_COMPLETED ? FDialogConditionString::CompletedQuest :
        RequirementType == EDialogConditionRequirementType::IS_COMPLETE ? FDialogConditionString::QuestReadyToTurnIn :
        FDialogConditionString::FailedQuest;

    SerializedCondition = SerializedCondition.Append(":").Append(Id.ToString());
    // negate of specified
    if (bNegate) SerializedCondition = FString("!").Append(SerializedCondition); 

    return SerializedCondition;
}

void UQuestConditionNode::UpdateNodeTitle()
{
    FString QuestId = Id.IsNone() ? "" : Id.ToString();

    if (bNegate)
    {
        NodeTitle = RequirementType == EDialogConditionRequirementType::HAVE ?
            FText::FromString(FString("NOT Have Quest").Append(": ").Append(QuestId)) : 
			RequirementType == EDialogConditionRequirementType::HAS_COMPLETED ?
            FText::FromString(FString("NOT Completed Quest").Append(": ").Append(QuestId)) :
			RequirementType == EDialogConditionRequirementType::IS_COMPLETE ?
			FText::FromString(FString("NOT Quest Ready to Turn In").Append(": ").Append(QuestId)) :
			FText::FromString(FString("NOT Failed Quest").Append(": ").Append(QuestId));
            
        return;
    }

    NodeTitle = RequirementType == EDialogConditionRequirementType::HAVE ?
        FText::FromString(FString("Have Quest").Append(": ").Append(QuestId)) :
		RequirementType == EDialogConditionRequirementType::HAS_COMPLETED ?
        FText::FromString(FString("Completed Quest").Append(": ").Append(QuestId)) :
		RequirementType == EDialogConditionRequirementType::IS_COMPLETE ?
		FText::FromString(FString("Quest Ready to Turn In").Append(": ").Append(QuestId)) :
		FText::FromString(FString("Failed Quest").Append(": ").Append(QuestId));

    return;
}
