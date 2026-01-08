// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"       
#include "IAssetTypeActions.h"     
#include "Factory/DialogNodeFactory.h"

class FRpgDialogSystemEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;

	TSharedPtr<IDetailsView> GraphDetailsView;

	TSharedPtr<FDialogNodeFactory> DialogNodeFactory;

	void OnNodeSelected(const TSet<UObject*>& SelectedObjects);

	virtual void ShutdownModule() override;

private:

	TArray<TSharedPtr<IAssetTypeActions>> RegisteredAssetTypeActions;
};
