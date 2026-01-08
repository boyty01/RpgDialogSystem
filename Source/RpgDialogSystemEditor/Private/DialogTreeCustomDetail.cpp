#include "DialogTreeCustomDetail.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"
#include "DataAsset/DialogTree.h" 

TSharedRef<IDetailCustomization> FDialogTreeCustomDetail::MakeInstance(EMenuMode InMenuMode)
{
	return MakeShareable(new FDialogTreeCustomDetail(InMenuMode));
}

void FDialogTreeCustomDetail::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{

    TArray<FName> CategoryNames;
    DetailBuilder.GetCategoryNames(CategoryNames);



    // Enable a subset based on the active menu
    switch (MenuMode)
    {
    case EMenuMode::StatesOnly:
    {
        for (FName CategoryName : CategoryNames)
        {
            if( CategoryName == "Dialog States")
				continue;

            DetailBuilder.HideCategory(CategoryName);
			UE_LOG(LogTemp, Warning, TEXT("Hiding category: %s"), *CategoryName.ToString());
        }
        
        break;
    }

    case EMenuMode::NoStates:
    {
        // Hide DialogStates
        TSharedRef<IPropertyHandle> DialogStatesProp = DetailBuilder.GetProperty(
            GET_MEMBER_NAME_CHECKED(UDialogTree, DialogStates)
        );
        DialogStatesProp->MarkHiddenByCustomization();
        break;
    }

    case EMenuMode::Everything:
    default:
        break;
    }
}