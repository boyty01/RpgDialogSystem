#include "DialogGraphEditorCommands.h"
#include "InputCoreTypes.h"

#define LOCTEXT_NAMESPACE "DialogGraphEditorCommands"


void FDialogGraphEditorCommands::RegisterCommands()
{
    UI_COMMAND(CreateComment, "Create Comment", "Add a comment box to the graph", EUserInterfaceActionType::Button, FInputChord(EKeys::C));

    UI_COMMAND(CopyNodes, "Copy", "Copy selected nodes",
        EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::C));

    UI_COMMAND(CutNodes, "Cut", "Cut selected nodes",
        EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::X));

    UI_COMMAND(PasteNodes, "Paste", "Paste nodes from clipboard",
        EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::V));

    UI_COMMAND(DuplicateNodes, "Duplicate", "Duplicate selected nodes",
        EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::D));

    UI_COMMAND(DeleteNodes, "Delete", "Delete selected nodes",
        EUserInterfaceActionType::Button, FInputChord(EKeys::Delete));
}


#undef LOCTEXT_NAMESPACE