// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/DialogEventNodeBase.h"
#include "DialogInventoryEventNode.generated.h"

UENUM()
enum class EDialogInventoryEventAction : uint8
{
	ADD_ITEMS,
	REMOVE_ITEMS
};
/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogInventoryEventNode : public UDialogEventNodeBase
{
	GENERATED_BODY()
public:

	UDialogInventoryEventNode();

	// the inventory action to make.
	UPROPERTY(EditAnywhere)
	EDialogInventoryEventAction InventoryAction;

	virtual void UpdateNodeTitle() override;

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual FSlateColor GetTitleBackgroundColor() const override;
	virtual void UpdateEventNodePostInit() override;
};
