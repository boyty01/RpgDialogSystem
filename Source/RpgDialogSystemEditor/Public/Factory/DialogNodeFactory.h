// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintGraphPanelPinFactory.h"
#include "Slate/DialogNodeStyle.h"

/**
 * 
 */
class RPGDIALOGSYSTEMEDITOR_API FDialogNodeFactory : public FGraphPanelNodeFactory
{
public:

    virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* InNode) const override;

};
