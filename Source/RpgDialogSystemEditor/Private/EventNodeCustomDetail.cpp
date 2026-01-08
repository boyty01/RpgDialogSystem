#include "EventNodeCustomDetail.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"
#include "Nodes/DialogEventNodeBase.h"
#include "DataAsset/DialogTree.h" 

TSharedRef<IDetailCustomization> FEventNodeCustomDetail::MakeInstance(EEventDetailsMode InEventDetailsMode)
{
	return MakeShareable(new FEventNodeCustomDetail(InEventDetailsMode));
}

void FEventNodeCustomDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{

    TArray<FName> CategoryNames;
    DetailBuilder.GetCategoryNames(CategoryNames);

    // always hide editor category on subnodes.
    DetailBuilder.HideCategory("Editor");

	// always hide target and underlying action type. child classes handle setting this and should not be modifiable.
    DetailBuilder.HideProperty(
        GET_MEMBER_NAME_CHECKED(UDialogEventNodeBase, ActionType)
    );

    DetailBuilder.HideProperty(
        GET_MEMBER_NAME_CHECKED(UDialogEventNodeBase, TargetSystem)
    );


    // Enable a subset based on the active menu
    switch (EventDetailsMode)
    {
    case EEventDetailsMode::StatesView:
    {
        break;
    }

    case EEventDetailsMode::QuestView:
    {  
        break;
    }

    case EEventDetailsMode::InventoryView:
    {
        break;
    }
    case EEventDetailsMode::CustomScriptView:
        DetailBuilder.HideProperty(
            GET_MEMBER_NAME_CHECKED(UDialogEventNodeBase, Id)
		);
        DetailBuilder.HideProperty(
            GET_MEMBER_NAME_CHECKED(UDialogEventNodeBase, Value)
        );
		break;

    case EEventDetailsMode::Everything:
    default:
        break;
    }
}