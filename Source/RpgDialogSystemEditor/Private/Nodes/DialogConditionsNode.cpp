// Copyright DMTesseracT LTd. All rights reserved.


#include "Nodes/DialogConditionsNode.h"
#include "Nodes/DialogGraphNode.h"
#include "Nodes/PlayerChoiceNode.h"

const FName UDialogConditionsNode::OutputPin_Conditions_Name = FName("OutConditions");

void UDialogConditionsNode::AllocateDefaultPins()
{
    // Create output pin for flow to next dialog event
    UEdGraphPin* NextPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, TEXT("RIGHT"), nullptr, OutputPin_Conditions_Name);
    NextPin->bNotConnectable = false;
    NextPin->bDefaultValueIsIgnored = true;
}

FPinConnectionResponse UDialogConditionsNode::CanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const
{
    if (OtherInputPin->GetOwningNode() && OtherInputPin->GetOwningNode()->IsA<UDialogGraphNode>() && OtherInputPin->PinName == UDialogGraphNode::InputPin_SelectionCondition_Name
        || OtherInputPin->GetOwningNode() && OtherInputPin->GetOwningNode()->IsA<UPlayerChoiceNode>() && OtherInputPin->PinName == UPlayerChoiceNode::InputPin_SelectionConditions_Name)
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, "Connect");
    }
    return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, "Incompatible");
}

void UDialogConditionsNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    UpdateNodeTitle();
    if (UEdGraph* Graph = GetGraph())
    {
        Graph->NotifyGraphChanged();
    }
    ValidateNodeConfiguration();
}

void UDialogConditionsNode::UpdateNodeTitle()
{

}
