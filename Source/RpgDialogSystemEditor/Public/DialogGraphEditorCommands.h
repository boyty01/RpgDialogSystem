#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"

class FDialogGraphEditorCommands : public TCommands<FDialogGraphEditorCommands>
{
public:
    FDialogGraphEditorCommands()
        : TCommands<FDialogGraphEditorCommands>(
            TEXT("DialogGraphEditor"), 
            NSLOCTEXT("Contexts", "DialogGraphEditor", "Dialog Graph Editor"),
            NAME_None,
            FAppStyle::GetAppStyleSetName())
    {
    }

    virtual void RegisterCommands() override;


public:

    TSharedPtr<FUICommandInfo> CreateComment;
    TSharedPtr<FUICommandInfo> CopyNodes;
    TSharedPtr<FUICommandInfo> CutNodes;
    TSharedPtr<FUICommandInfo> PasteNodes;
    TSharedPtr<FUICommandInfo> DuplicateNodes;
    TSharedPtr<FUICommandInfo> DeleteNodes;

};