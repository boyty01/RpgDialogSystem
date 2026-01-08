// Copyright DMTesseracT LTd. All rights reserved.


#include "TypeActions/DialogTreeAssetTypeActions.h"
#include "DataAsset/DialogTree.h"
#include "Script/DialogScriptEventBase.h"
#include "DialogTreeEditorToolkit.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Factory/DialogCustomScriptFactory.h"
#include "Kismet2/KismetEditorUtilities.h"

/*
// DIALOG TREE ASSET

UClass* FDialogTreeAssetTypeActions::GetSupportedClass() const
{
	return UDialogTree::StaticClass();
}

void FDialogTreeAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
    for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
    {
        if (UDialogTree* DialogTree = Cast<UDialogTree>(*ObjIt))
        {
          TSharedRef<FDialogTreeEditorToolkit> EditorToolkit = MakeShared<FDialogTreeEditorToolkit>();
            EditorToolkit->Initialize(DialogTree, EToolkitMode::Standalone, EditWithinLevelEditor);
        }
    }
}
*/

EAssetCommandResult UDialogTreeAssetDefinition::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
    EAssetCommandResult result = EAssetCommandResult::Unhandled;
    for (auto AssetData: OpenArgs.Assets)
    {

        if (UDialogTree* DialogTree = Cast<UDialogTree>(AssetData.GetAsset()))
        {
            TSharedRef<FDialogTreeEditorToolkit> EditorToolkit = MakeShared<FDialogTreeEditorToolkit>();
            EditorToolkit->Initialize(DialogTree, EToolkitMode::Standalone, OpenArgs.ToolkitHost);
            result = EAssetCommandResult::Handled;
        }
    }
    return result;
}


