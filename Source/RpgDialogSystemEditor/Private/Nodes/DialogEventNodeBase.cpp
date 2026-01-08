// Copyright DMTesseracT LTd. All rights reserved.


#include "Nodes/DialogEventNodeBase.h"
#include "Nodes/DialogGraphNode.h"
#include "Nodes/PlayerChoiceNode.h"


void UDialogEventNodeBase::AllocateDefaultPins()
{
    // Create output pin for flow to next dialog event
    UEdGraphPin* NextPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, TEXT("RIGHT"), nullptr, OutputPin_Event_Name);
    NextPin->bNotConnectable = false;
    NextPin->bDefaultValueIsIgnored = true;
}

FPinConnectionResponse UDialogEventNodeBase::CanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const
{
    if (OtherInputPin->GetOwningNode() && OtherInputPin->GetOwningNode()->IsA<UDialogGraphNode>() && OtherInputPin->PinName == UDialogGraphNode::InputPin_Event_Name
        || OtherInputPin->GetOwningNode() && OtherInputPin->GetOwningNode()->IsA<UPlayerChoiceNode>() && OtherInputPin->PinName == UPlayerChoiceNode::InputPin_Events_Name)
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, "Connect");
    }
    return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, "Incompatible");
}

void UDialogEventNodeBase::ValidateNodeConfiguration()
{
    if (Id.IsNone())
    {
        ConfigurationState = EDialogGraphConfigurationState::Warning;
        ErrorMsg = TEXT("Event Node has no Key specified.");
        return;
    }
    ConfigurationState = EDialogGraphConfigurationState::Valid;
	ErrorMsg = "";
}

void UDialogEventNodeBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    UpdateNodeTitle();
    if (UEdGraph* Graph = GetGraph())
    {
        Graph->NotifyGraphChanged();
    }
}
