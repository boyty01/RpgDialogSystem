// Copyright DMTesseracT LTd. All rights reserved.


#include "Nodes/PlayerChoiceNode.h"

const FName UPlayerChoiceNode::InputPin_InDialog_Name = FName("InDialog");
const FName UPlayerChoiceNode::OutputPin_OutDialog_Name = FName("TriggersDialog");
const FName UPlayerChoiceNode::InputPin_Events_Name = FName("Events");
const FName UPlayerChoiceNode::InputPin_SelectionConditions_Name = FName("In Conditions");

FPinConnectionResponse UPlayerChoiceNode::CanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const
{
    if (OtherInputPin->GetOwningNode() && OtherInputPin->GetOwningNode()->IsA<UDialogGraphNode>())
    {
        if (OtherInputPin->PinName == UDialogGraphNode::InputPin_InPlayerChoice_Name)
        {
            if (MyOutputPin->HasAnyConnections())
            {
                return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT("Replace output connection"));
            }
            return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Connect"));
        }
    }
    return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, FText::FromString("Incompatible"));
}

void UPlayerChoiceNode::AllocateDefaultPins()
{
    UEdGraphPin* InPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, TEXT("TOP"), nullptr, InputPin_InDialog_Name);
    InPin->bNotConnectable = false;
    InPin->bDefaultValueIsIgnored = true;

    UEdGraphPin* InConPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, TEXT("LEFT"), nullptr, InputPin_SelectionConditions_Name);
    InConPin->bNotConnectable = false;
    InConPin->bDefaultValueIsIgnored = true;

    UEdGraphPin* InEventPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, TEXT("LEFT"), nullptr, InputPin_Events_Name);
    InEventPin->bNotConnectable = false;
    InEventPin->bDefaultValueIsIgnored = true;
   
    UEdGraphPin* OutPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, TEXT("BOTTOM"), nullptr, OutputPin_OutDialog_Name);
    OutPin->bNotConnectable = false;
    OutPin->bDefaultValueIsIgnored = true;
}

void UPlayerChoiceNode::NodeConnectionListChanged()
{
    Super::NodeConnectionListChanged();

    if (UEdGraph* Graph = GetGraph())
    {
        Graph->NotifyNodeChanged(this);
    }
}


void UPlayerChoiceNode::ValidateNodeConfiguration()
{
    ErrorMsg = "";
    ConfigurationState = EDialogGraphConfigurationState::Valid;

    if (ResponseText.IsEmpty())
    {
        ConfigurationState = EDialogGraphConfigurationState::Warning;
        ErrorMsg = TEXT("- Response Text is empty.");
    }

    ValidateSubNodes();

    // set state to the highest error state of subnodes, if they have a state higher than this node.
    if (ConfigurationState < GetHighestErrorStateInSubNodes())
    {
        ConfigurationState = GetHighestErrorStateInSubNodes();
    }

    if (ErrorMsg.Len() > 0)
    {
        ErrorMsg.Append(TEXT("\n\n"));
    }

    if (GetHighestErrorStateInSubNodes() == EDialogGraphConfigurationState::Warning)
    {
        ErrorMsg.Append(TEXT("- One or more sub-nodes have warnings."));
        return;
    }

    if (GetHighestErrorStateInSubNodes() == EDialogGraphConfigurationState::Error)
    {
        ErrorMsg.Append(TEXT("- One or more sub-nodes have errors."));
        return;
    }

}

void UPlayerChoiceNode::ValidateSubNodes()
{
    TArray<UDialogEdGraphNodeBase*> Subnodes = GetSubNodesOfType<UDialogEdGraphNodeBase>();
    EDialogGraphConfigurationState HighestState = EDialogGraphConfigurationState::Valid;

    for (UDialogEdGraphNodeBase* Subnode : Subnodes)
    {
        Subnode->ValidateNodeConfiguration();
    }

}

EDialogGraphConfigurationState UPlayerChoiceNode::GetHighestErrorStateInSubNodes() 
{
    TArray<UDialogEdGraphNodeBase*> Subnodes = GetSubNodesOfType<UDialogEdGraphNodeBase>();
    EDialogGraphConfigurationState HighestState = EDialogGraphConfigurationState::Valid;

    for (UDialogEdGraphNodeBase* Subnode : Subnodes)
    {
        if (Subnode->ConfigurationState > HighestState)
        {
            HighestState = Subnode->ConfigurationState;
        }
    }
    return HighestState;
}
