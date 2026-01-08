// Copyright DMTesseracT LTd. All rights reserved.


#include "Nodes/InventoryConditionNode.h"
#include "DialogTypes.h"
FString UInventoryConditionNode::GetSerializedString() const
{
    FString SerializedString;
    SerializedString = FString::Format(TEXT("{0}:{1}"), { FDialogConditionString::HasItem, Id.ToString() });
    if (NumberRequired> 1) SerializedString = FString::Format(TEXT("{0}={1}"), { SerializedString, FString::FromInt(NumberRequired) }); // if multi value required
    if (bNegate) SerializedString = FString::Format(TEXT("{0}{1}"), { "!", SerializedString }); // if negated
    return SerializedString;
}

void UInventoryConditionNode::ValidateNodeConfiguration()
{
    if (Id.IsNone())
    {
        ConfigurationState = EDialogGraphConfigurationState::Warning;
		ErrorMsg = TEXT("Inventory Condition Node has no Item ID specified.");
    }
    else
    {
        ConfigurationState = EDialogGraphConfigurationState::Valid;
        ErrorMsg = "";
	}
}

void UInventoryConditionNode::UpdateNodeTitle()
{
    FString Title;
    FString NumString = FString::FromInt(NumberRequired);
    if (bNegate)
    {
        NodeTitle = NumberRequired > 1 ?
            FText::FromString(FString::Format(TEXT("Must Have Less Than {0} of Item: {1}"), { NumString, Id.ToString() })) :
            FText::FromString(FString::Format(TEXT("Must NOT Have Item: {0}"), { Id.ToString() }));
        return;
    }

    NodeTitle = NumberRequired > 1 ?
        FText::FromString(FString::Format(TEXT("Must Have Atleast {0} of Item: {1}"), { NumString, Id.ToString() })) :
        FText::FromString(FString::Format(TEXT("Must Have Item: {0}"), { Id.ToString() }));

    return;
}

