// Copyright DMTesseracT LTd. All rights reserved.


#include "Nodes/ReputationConditionNode.h"


void UReputationConditionNode::ValidateNodeConfiguration()
{
    if (Id.IsNone())
    {
        ConfigurationState = EDialogGraphConfigurationState::Warning;
        ErrorMsg = TEXT("Reputation Condition Node has no Reputation ID specified.");
    }
    else
    {
        ConfigurationState = EDialogGraphConfigurationState::Valid;
        ErrorMsg = "";
	}
}

FString UReputationConditionNode::GetSerializedString() const
{
    FString SerializedCondition;
    SerializedCondition = RequirementType == EDialogConditionRequirementType::AT_LEAST ? FDialogConditionString::HasReputation : FDialogConditionString::HasLessThanReputation;

    SerializedCondition = SerializedCondition.Append(":").Append(Id.ToString());
    // negate of specified
    if (bNegate) SerializedCondition = FString("!").Append(SerializedCondition); 

    return SerializedCondition;
}

void UReputationConditionNode::UpdateNodeTitle()
{
    FString ReputationId = Id.IsNone() ? "" : Id.ToString();

    if (bNegate)
    {
        NodeTitle = RequirementType == EDialogConditionRequirementType::AT_LEAST ?
            FText::FromString(FString("NOT Have Reputation").Append(": ").Append(ReputationId)) :
            FText::FromString(FString("NOT Have Less Than Reputation").Append(": ").Append(ReputationId));
            
        return;
    }

    NodeTitle = RequirementType == EDialogConditionRequirementType::AT_LEAST ?
        FText::FromString(FString("Have Reputation").Append(": ").Append(ReputationId)) :
        FText::FromString(FString("Have Less Than Reputation").Append(": ").Append(ReputationId));

    return;
}
