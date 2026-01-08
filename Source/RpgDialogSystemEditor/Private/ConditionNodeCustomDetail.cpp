#include "ConditionNodeCustomDetail.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"
#include "Nodes/DialogConditionsNode.h"
#include "DataAsset/DialogTree.h" 

TSharedRef<IDetailCustomization> FConditionNodeCustomDetail::MakeInstance(EConditionDetailsMode InEventDetailsMode)
{
	return MakeShareable(new FConditionNodeCustomDetail(InEventDetailsMode));
}

void FConditionNodeCustomDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{

    TArray<FName> CategoryNames;
    DetailBuilder.GetCategoryNames(CategoryNames);

    for(FName CategoryName : CategoryNames)
    {
        UE_LOG(LogTemp, Warning, TEXT("Category: %s"), *CategoryName.ToString());
	}

    
    // always hide editor category on subnodes.
    DetailBuilder.HideCategory("Editor");

	// always hide target and underlying action type. child classes handle setting this and should not be modifiable.
    DetailBuilder.HideProperty(
        GET_MEMBER_NAME_CHECKED(UDialogConditionsNode, ConditionTarget)
    );




    // Enable a subset based on the active menu
    switch (ConditionDetailsMode)
    {
    case EConditionDetailsMode::StatesView:
    {
        DetailBuilder.HideProperty(
            GET_MEMBER_NAME_CHECKED(UDialogConditionsNode, RequirementType)
        );
        DetailBuilder.HideProperty(
            GET_MEMBER_NAME_CHECKED(UDialogConditionsNode, NumberRequired)
        );
        break;
    }

    case EConditionDetailsMode::QuestView:
    {  
        DetailBuilder.HideProperty(
            GET_MEMBER_NAME_CHECKED(UDialogConditionsNode, NumberRequired)
        );
        break;
    }

    case EConditionDetailsMode::InventoryView:
    {
        DetailBuilder.HideProperty(
            GET_MEMBER_NAME_CHECKED(UDialogConditionsNode, RequirementType)
        );
        break;
    }

    case EConditionDetailsMode::Everything:
    default:
        break;
    }
}