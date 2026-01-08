// Copyright DMTesseracT LTd. All rights reserved.


#include "Nodes/DialogInventoryEventNode.h"

UDialogInventoryEventNode::UDialogInventoryEventNode()
{
	TargetSystem = EDialogSystemTarget::INVENTORY;
	NodeTitle = FText::FromString("Inventory Event Node");
	bCanEditValue = true;
}

void UDialogInventoryEventNode::UpdateNodeTitle()
{
	FString ActionString = InventoryAction == EDialogInventoryEventAction::ADD_ITEMS ? TEXT("Add") : TEXT("Remove");
	NodeTitle = FText::FromString(FString::Format(TEXT("{0} {1}x of Item:{2}"), {ActionString, FString::FromInt(Value), Id.ToString()}));
}

void UDialogInventoryEventNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	//manually clamp inventory events to min 1 item.
	if (Value < 1) Value = 1;

	switch (InventoryAction)
	{
	case EDialogInventoryEventAction::ADD_ITEMS :
		ActionType = EDialogActionType::ADD_ITEM;
		break;
	case EDialogInventoryEventAction::REMOVE_ITEMS:
		ActionType = EDialogActionType::REMOVE_ITEM;
		break;
	default:
		ActionType = EDialogActionType::ADD_ITEM;
		break;
	}
}

FSlateColor UDialogInventoryEventNode::GetTitleBackgroundColor() const
{
	if (InventoryAction == EDialogInventoryEventAction::ADD_ITEMS)
	{
		return FLinearColor(0.7f, 0.7f, 0.7f, 0.7f);
	}

	if (InventoryAction == EDialogInventoryEventAction::REMOVE_ITEMS)
	{
		return FLinearColor(0.7f, 0.7f, 0.7f, 0.7f);
	}

	return FLinearColor::White;
}

void UDialogInventoryEventNode::UpdateEventNodePostInit()
{
	switch (ActionType)
	{
	case EDialogActionType::ADD_ITEM:
		InventoryAction = EDialogInventoryEventAction::ADD_ITEMS;
		break;
	default:
		InventoryAction = EDialogInventoryEventAction::REMOVE_ITEMS;
	}
}
