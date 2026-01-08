// Copyright DMTesseracT LTd. All rights reserved.


#include "Nodes/DialogGraphNode.h"
#include "Nodes/PlayerChoiceNode.h"
#include "LevelSequence.h"

const FName UDialogGraphNode::InputPin_Event_Name = FName("InEvents");
const FName UDialogGraphNode::InputPin_InDialog_Name = FName("InDialog");
const FName UDialogGraphNode::InputPin_InPlayerChoice_Name = FName("InChoice");
const FName UDialogGraphNode::InputPin_SelectionCondition_Name = FName("InConditions");
const FName UDialogGraphNode::OutputPin_OutDialog_Name = FName("NextDialog");
const FName UDialogGraphNode::OutputPin_OutPlayerChoice_Name = FName("Player Choices");

UDialogGraphNode::UDialogGraphNode()
{

}

FPinConnectionResponse UDialogGraphNode::CanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const
{
    UEdGraphNode* OtherNode = OtherInputPin->GetOwningNode();

    // general unsupported 
    if (!OtherNode)
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Unsupported Pin"));
    }

    if (OtherNode == this)
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Cannot connect to self."));
    }

    if (MyOutputPin->PinName == UDialogGraphNode::OutputPin_OutDialog_Name)
    {
        return Internal_NextDialogCanLinkTo(MyOutputPin, OtherInputPin);
    }

    if (MyOutputPin->PinName == UDialogGraphNode::OutputPin_OutPlayerChoice_Name)
    {
        return Internal_PlayerChoicesCanLinkTo(MyOutputPin, OtherInputPin);
    }

    return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Unsupported Pin"));
}

void UDialogGraphNode::AllocateDefaultPins()
{

    UEdGraphPin* InPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, TEXT("TOP"), nullptr, InputPin_InDialog_Name);
    InPin->bNotConnectable = false;
    InPin->bDefaultValueIsIgnored = true;

    UEdGraphPin* InChoicePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, TEXT("TOP"), nullptr, InputPin_InPlayerChoice_Name);
    InChoicePin->bNotConnectable = false;
    InChoicePin->bDefaultValueIsIgnored = true;

    UEdGraphPin* InSelectionConditionsPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, TEXT("LEFT"), nullptr, InputPin_SelectionCondition_Name);
    InSelectionConditionsPin->bNotConnectable = false;
    InSelectionConditionsPin->bDefaultValueIsIgnored = true;

    UEdGraphPin* InSequenceEventPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, TEXT("LEFT"), nullptr, InputPin_Event_Name);
    InSequenceEventPin->bNotConnectable = false;
    InSequenceEventPin->bDefaultValueIsIgnored = true;

    // Create output pin for flow to next dialog event
    UEdGraphPin* NextPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, TEXT("BOTTOM"), nullptr, OutputPin_OutDialog_Name);
    NextPin->bNotConnectable = false;
    NextPin->bDefaultValueIsIgnored = true;

    UEdGraphPin* ChoicesPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, TEXT("BOTTOM"), nullptr, OutputPin_OutPlayerChoice_Name);
    ChoicesPin->bNotConnectable = false;
    ChoicesPin->bDefaultValueIsIgnored = true;


}

void UDialogGraphNode::ValidateNodeConfiguration()
{
    ErrorMsg = "";
	ConfigurationState = EDialogGraphConfigurationState::Valid;

    if (LevelSequenceToPlay.IsNull())
    {
        ConfigurationState = EDialogGraphConfigurationState::Error;
		ErrorMsg = TEXT("- No Level Sequence is assigned to play during this dialog sequence.");
    }

    ValidateSubNodes();

	// set state to the highest error state of subnodes, if they have a state higher than this node.
    if (ConfigurationState < GetHighestErrorStateInSubNodes())
    {
        ConfigurationState = GetHighestErrorStateInSubNodes();
    }

    if(ErrorMsg.Len() > 0)
    {
        ErrorMsg.Append(TEXT("\n\n"));
	}

    if(GetHighestErrorStateInSubNodes() == EDialogGraphConfigurationState::Warning)
    {
        ErrorMsg.Append(TEXT("- One or more sub-nodes have warnings."));
        return;
    }
        
    if(GetHighestErrorStateInSubNodes() == EDialogGraphConfigurationState::Error)
    {
        ErrorMsg.Append(TEXT("- One or more sub-nodes have errors."));
        return;
	}

}

void UDialogGraphNode::ValidateSubNodes()
{
    TArray<UDialogEdGraphNodeBase*> Subnodes = GetSubNodesOfType<UDialogEdGraphNodeBase>();
    EDialogGraphConfigurationState HighestState = EDialogGraphConfigurationState::Valid;

    for(UDialogEdGraphNodeBase* Subnode : Subnodes)
    {
        Subnode->ValidateNodeConfiguration();
	}



}

EDialogGraphConfigurationState UDialogGraphNode::GetHighestErrorStateInSubNodes()
{
	TArray<UDialogEdGraphNodeBase*> Subnodes = GetSubNodesOfType<UDialogEdGraphNodeBase>();
	EDialogGraphConfigurationState HighestState = EDialogGraphConfigurationState::Valid;

    for(UDialogEdGraphNodeBase* Subnode : Subnodes)
    {
        if (Subnode->ConfigurationState > HighestState)
        {
            HighestState = Subnode->ConfigurationState;
        }
	}
    return HighestState;
}

void UDialogGraphNode::NodeConnectionListChanged()
{
    Super::NodeConnectionListChanged();
    if (UEdGraph* Graph = GetGraph())
    {
        Graph->NotifyNodeChanged(this);
    }
}

FPinConnectionResponse UDialogGraphNode::Internal_NextDialogCanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const
{
    if (OtherInputPin->GetOwningNode() && OtherInputPin->GetOwningNode()->IsA(UDialogGraphNode::StaticClass()) && OtherInputPin->PinName == UDialogGraphNode::InputPin_InDialog_Name)
    {
        if (MyOutputPin->HasAnyConnections())
        {
            return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, FText::FromString("Replace connection"));
        }

        return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, FText::FromString("Connect"));
    }

    return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, FText::FromString("Incompatible"));
}

FPinConnectionResponse UDialogGraphNode::Internal_PlayerChoicesCanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const
{
    if (OtherInputPin->GetOwningNode() && OtherInputPin->GetOwningNode()->IsA(UPlayerChoiceNode::StaticClass()) && OtherInputPin->PinName == UPlayerChoiceNode::InputPin_InDialog_Name)
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, FText::FromString("Connect"));
    }

    return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, FText::FromString("Incompatible"));
}
