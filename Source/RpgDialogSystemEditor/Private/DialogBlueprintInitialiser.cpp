#include "DialogBlueprintInitialiser.h"


#include "Kismet2/BlueprintEditorUtils.h"
#include "K2Node_Event.h"
#include "Script/DialogScriptEventBase.h"
#include "EdGraphSchema_K2_Actions.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "Kismet2/KismetEditorUtilities.h"


void FDialogBlueprintInitialiser::OnAssetEditorOpened(UObject* Asset)
{
    UBlueprint* Blueprint = Cast<UBlueprint>(Asset);
    if (!Blueprint)
        return;


	// Handle Dialog Script Event Blueprints
    if (Blueprint->ParentClass->IsChildOf(UDialogScriptEventBase::StaticClass()))
    {
		OnDialogEventBlueprintEditorOpened(Blueprint);
    }


}

void FDialogBlueprintInitialiser::OnDialogEventBlueprintEditorOpened(UBlueprint* Blueprint)
{
    // Find the EventGraph
    UEdGraph* EventGraph = FBlueprintEditorUtils::FindEventGraph(Blueprint);
    if (!EventGraph)
    {
        return;
    }

	// Add ExecuteScript as a default event if it doesn't already exist. Note we don't check whether the event has been deleted before, and will always try to add it if it's missing.

    const FName EventName =
        GET_FUNCTION_NAME_CHECKED(UDialogScriptEventBase, ExecuteScript);

    // If an override already exists, stop here.
    if (FBlueprintEditorUtils::FindOverrideForFunction(
        Blueprint,
        UDialogScriptEventBase::StaticClass(),
        EventName))
    {
        return;
    }

    int32 PosY = 0;

	// spawn the execute script event node
    UK2Node_Event* EventNode =
        FKismetEditorUtilities::AddDefaultEventNode(
            Blueprint,
            EventGraph,
            EventName,
            UDialogScriptEventBase::StaticClass(),
            PosY
        );

	// safety net. should not be null here.
    if (!EventNode)
    {
        return;
    }



	
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
}
