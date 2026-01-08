// Copyright Epic Games, Inc. All Rights Reserved.

#include "RpgDialogSystemEditor.h"
#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "Graph/DialogGraph.h"
#include "LevelEditor.h"
#include "Schema/DialogGraphSchema.h"
#include "UI/DialogGraphEditor.h"
#include "Factory/DialogNodeFactory.h"
#include "TypeActions/DialogTreeAssetTypeActions.h"
#include "DialogGraphEditorCommands.h"
#include "RpgDialogSlateStyle.h"
#include "DialogBlueprintInitialiser.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include <EdGraphUtilities.h>

#define LOCTEXT_NAMESPACE "FRpgDialogSystemModule"


FName TabName = FName("DialogEditorTab");

void FRpgDialogSystemEditorModule::StartupModule()
{
	FRpgDialogSlateStyle::Initialize();
    FDialogGraphEditorCommands::Register();
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

    // Register a new category
    EAssetTypeCategories::Type RPGDialogCategory = AssetTools.RegisterAdvancedAssetCategory(
        FName("RPGDialog"),
        NSLOCTEXT("AssetCategory", "RPGDialogCategory", "RPG Dialog")
    );

    /*
    TSharedRef<IAssetTypeActions> DialogTreeAssetActions = MakeShareable(new FDialogTreeAssetTypeActions(RPGDialogCategory));
    AssetTools.RegisterAssetTypeActions(DialogTreeAssetActions);
         
    RegisteredAssetTypeActions.Add(DialogTreeAssetActions);
    */

    DialogNodeFactory = MakeShared<FDialogNodeFactory>();
    FEdGraphUtilities::RegisterVisualNodeFactory(DialogNodeFactory);
   
#if WITH_EDITOR
    // Delay binding until editor is fully initialized
    FEditorDelegates::OnEditorInitialized.AddLambda([](double Duration)
        {
            if (GEditor)
            {
                if (UAssetEditorSubsystem* AssetEditorSubsystem =
                    GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
                {
                    // Bind your static handler
                    AssetEditorSubsystem->OnAssetEditorOpened().AddStatic(
                        &FDialogBlueprintInitialiser::OnAssetEditorOpened
                    );

                    // Remove this lambda so it runs only once
                    FEditorDelegates::OnEditorInitialized.RemoveAll(nullptr);
                }
            }
        });
#endif
}

TSharedPtr<SGraphEditor> GraphEditor;




void FRpgDialogSystemEditorModule::OnNodeSelected(const TSet<UObject*>& SelectedObjects)
{
    if (SelectedObjects.Num() == 1)
    {
        if (UDialogGraphNode* Node = Cast<UDialogGraphNode>(*SelectedObjects.CreateConstIterator()))
        {
            GraphDetailsView->SetObject(Node);
        }
    }
    else
    {
        GraphDetailsView->SetObject(nullptr);
    }
}



void FRpgDialogSystemEditorModule::ShutdownModule()
{
	FRpgDialogSlateStyle::Shutdown();
    FDialogGraphEditorCommands::Unregister();
    if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
    {
        IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
        for (auto& Action : RegisteredAssetTypeActions)
        {
            AssetTools.UnregisterAssetTypeActions(Action.ToSharedRef());
        }
    }
    // Unregister all registered asset type actions

    RegisteredAssetTypeActions.Empty();

    if (DialogNodeFactory.IsValid())
    {
        FEdGraphUtilities::UnregisterVisualNodeFactory(DialogNodeFactory);
        DialogNodeFactory.Reset();
    }

	// remove blueprint editor opened event binding.
    if (GEditor)
    {
        if (UAssetEditorSubsystem* AssetEditorSubsystem =
            GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
        {
            AssetEditorSubsystem->OnAssetEditorOpened().RemoveAll(
                &FDialogBlueprintInitialiser::OnAssetEditorOpened
            );
        }
    }

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRpgDialogSystemEditorModule, RpgDialogSystemEditor)