// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/DialogEdGraphNodeBase.h"
#include "DialogTypes.h"
#include "Nodes/DialogEventNodeBase.h"
#include "Script/DialogScriptEventBase.h"
#include "DialogCustomEventNode.generated.h"

/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogCustomEventNode : public UDialogEventNodeBase
{
	GENERATED_BODY()
public:

	UDialogCustomEventNode();
	virtual void UpdateNodeTitle() override;
	virtual FSlateColor GetTitleBackgroundColor() const override;
	virtual void UpdateEventNodePostInit() override {};

	/* Custom event script class to be spawned. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Custom Event")
	TSubclassOf<UDialogScriptEventBase> CustomEventScriptClass;
	virtual void ValidateNodeConfiguration() override;
};
