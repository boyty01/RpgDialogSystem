// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "DialogEditorSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEditorSelectionChanged, const TArray<UObject*>&, Selections, bool, bForceRefresh);

/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
public:

	void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY(BlueprintAssignable, Category = "Editor Helpers")
	FOnEditorSelectionChanged OnEditorSelectionChanged;

	UFUNCTION(BlueprintCallable, Category = "Utility Helpers")
	void BindSelectEvent();

private:
	UFUNCTION()
	void EditorSelectionChanged(const TArray<UObject*>& Selections, bool bForceRefresh) { OnEditorSelectionChanged.Broadcast(Selections, bForceRefresh); };

};
