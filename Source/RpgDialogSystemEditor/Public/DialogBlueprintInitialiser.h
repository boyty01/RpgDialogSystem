#pragma once

class UBlueprint;

class FDialogBlueprintInitialiser
{
public:
    static void OnAssetEditorOpened(UObject* Asset);

	static void OnDialogEventBlueprintEditorOpened(UBlueprint* Blueprint);
};