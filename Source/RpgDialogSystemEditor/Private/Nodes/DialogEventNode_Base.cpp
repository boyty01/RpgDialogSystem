// Copyright DMTesseracT LTd. All rights reserved.


#include "Nodes/DialogEventNode_Base.h"
#include "Nodes/DialogGraphNode.h"
#include "Nodes/PlayerChoiceNode.h"

FPinConnectionResponse UDialogEventNode_Base::CanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const
{
    if(OtherInputPin->GetOwningNode()->IsA(UDialogGraphNode::StaticClass()))
    {
        UDialogGraphNode* DialogNode = Cast<UDialogGraphNode>(OtherInputPin->GetOwningNode());
        if (OtherInputPin->PinName.IsEqual(UDialogGraphNode::InputPin_Event_Name))
        {
            return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, "Add Event");
        }
    }

    if (OtherInputPin->GetOwningNode()->IsA(UPlayerChoiceNode::StaticClass()))
    {
        UPlayerChoiceNode* DialogNode = Cast<UPlayerChoiceNode>(OtherInputPin->GetOwningNode());
        if (OtherInputPin->PinName.IsEqual(UPlayerChoiceNode::InputPin_Events_Name))
        {
            return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, "Add Event");
        }
    }

    return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, "Incompatible");
}

void UDialogEventNode_Base::AllocateDefaultPins()
{  
    UEdGraphPin* OutPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FName(), nullptr, TEXT("Id"));
    OutPin->bNotConnectable = false;
    OutPin->bDefaultValueIsIgnored = true;
}
