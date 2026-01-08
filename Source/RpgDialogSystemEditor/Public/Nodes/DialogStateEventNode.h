// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/DialogEventNodeBase.h"
#include "DialogStateEventNode.generated.h"

/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogStateEventNode : public UDialogEventNodeBase
{
	GENERATED_BODY()
	
public:

	UDialogStateEventNode();

	virtual void UpdateNodeTitle() override;

	virtual void UpdateEventNodePostInit() override;

	virtual void ValidateNodeConfiguration() override;
};
