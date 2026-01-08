// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "SGraphPin.h"
#include "Nodes/DialogEdGraphNodeBase.h"
#include "Nodes/DialogGraphNode.h"
#include "Nodes/DialogEventNodeBase.h"
#include "Nodes/DialogConditionsNode.h"
#include "Graph/DialogGraph.h"

/**
 * 
 */
class RPGDIALOGSYSTEMEDITOR_API SDialogNodeStyle : public SGraphNode 
{
public:
    SLATE_BEGIN_ARGS(SDialogNodeStyle) {}
        SLATE_ARGUMENT(UDialogGraphNode*, GraphNode)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    void SetOwner(const TSharedRef<SGraphPanel>& InOwnerPanel) override;

    /* State dependent color getters. Used to allow changes to different colours based on local states */
    virtual FSlateColor GetBorderBackgroundColor() const;
    virtual FSlateColor GetBackgroundColor() const;
    virtual FSlateColor GetTitleBackgroundColor() const;
    virtual FSlateColor GetAltBackgroundColor() const;
    virtual FSlateColor GetAltBorderBackgroundColor() const;
    virtual FSlateColor GetAltTitleBackgroundColor() const;

	EVisibility GetValidationVisibility(const FString Icon) const;

    virtual void UpdateGraphNode() override;

    // Override CreatePinWidgets to generate pins from GraphNode
    virtual void CreatePinWidgets() override;


    //build out any subnode widgets
    virtual void BuildSubnodeWidgets();
    virtual void BuildConditionNode(UDialogConditionsNode* Node);
    virtual void BuildEventNode(UDialogEventNodeBase* Node);

    TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* Pin) const override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    
    /* Callback for subnodes when they're clicked. Should be passed to child node constructors */
    virtual void OnSubnodeSelected(UEdGraphNode* SubNode);



private:


    // Helper to toggle expand/collapse
    FReply OnExpandClicked();

    // Cached pointer to dialog node (for easy access)
    UDialogGraphNode* DialogNode;

    // Widget to show dialog line text
    TSharedPtr<STextBlock> DialogLineTextWidget;

    TArray<TSharedRef<SGraphPin>> LocalPinWidgets;

    TSharedPtr<SGraphPin> InDialogPin;
    TSharedPtr<SGraphPin> InChoicePin;

    TArray<TSharedPtr<SGraphNode>> SubNodes;
    TSharedPtr<SVerticalBox> ConditionWidgets;
    TSharedPtr<SVerticalBox> EventWidgets;
    TSharedPtr<SHorizontalBox> TopNodeBox;
    TSharedPtr<SHorizontalBox> BottomNodeBox;
};
