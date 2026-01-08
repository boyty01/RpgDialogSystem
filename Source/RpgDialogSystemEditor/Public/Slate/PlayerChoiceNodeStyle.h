// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "SGraphPin.h"
#include "Nodes/PlayerChoiceNode.h"

/**
 * 
 */
class RPGDIALOGSYSTEMEDITOR_API SPlayerChoiceNodeStyle : public SGraphNode
{
public:
    SLATE_BEGIN_ARGS(SPlayerChoiceNodeStyle) {}
        SLATE_ARGUMENT(UPlayerChoiceNode*, GraphNode)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    virtual FSlateColor GetAltBackgroundColor() const;
    virtual FSlateColor GetBorderBackgroundColor() const { return FLinearColor::Red; };
    virtual FSlateColor GetBackgroundColor() const { return FLinearColor::Red; };

    virtual void SetOwner(const TSharedRef<SGraphPanel>& InOwnerPanel) override;

    // Override UpdateGraphNode to rebuild the node UI
    virtual void UpdateGraphNode() override;

    // Override AddPin to add pins to the right container
    virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

    // Override CreatePinWidgets to generate pins from GraphNode
    virtual void CreatePinWidgets() override;

    //build out any subnode widgets
    virtual void BuildSubnodeWidgets();
    virtual void BuildConditionNode(UDialogConditionsNode* Node);
    virtual void BuildEventNode(class UDialogEventNodeBase* Node);


    TArray<TSharedPtr<SGraphNode>> SubNodes;
    TSharedPtr<SVerticalBox> ConditionWidgets;
    TSharedPtr<SVerticalBox> EventWidgets;
    TSharedPtr<SHorizontalBox> TopNodeBox;
    TSharedPtr<SHorizontalBox> BottomNodeBox;
    TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* Pin) const override;
private:

    EVisibility GetValidationVisibility(const FString Icon) const;

    // Cached pointer to dialog node (for easy access)
    UPlayerChoiceNode* ChoiceNode;

    // Widget to show dialog line text
    TSharedPtr<STextBlock> DialogLineTextWidget;


    TArray<TSharedRef<SGraphPin>> LocalPinWidgets;
};
