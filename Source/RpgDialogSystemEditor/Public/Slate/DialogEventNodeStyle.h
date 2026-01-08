// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "SGraphPin.h"
#include "Nodes/DialogEventNodeBase.h"

/**
 * 
 */
class RPGDIALOGSYSTEMEDITOR_API SDialogEventNodeStyle : public SGraphNode
{
public:
    SLATE_BEGIN_ARGS(SDialogEventNodeStyle) {}
        SLATE_ARGUMENT(UDialogEventNodeBase*, GraphNode)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FSlateColor GetBorderBackgroundColor() const {
        if (bIsSelected)
        {
            if (GetValidationVisibility("Warning") == EVisibility::Visible)
                return FLinearColor::Yellow;

            if (GetValidationVisibility("Error") == EVisibility::Visible)
                return FLinearColor::Red;

            return FLinearColor::Green;
        }

        return FLinearColor(1.f, 1.f, 1.f, 0.0f);
    };
    virtual FSlateColor GetBackgroundColor() const { return FLinearColor::Red; };
    // Override UpdateGraphNode to rebuild the node UI
    virtual void UpdateGraphNode() override;

    virtual void Tick(const FGeometry& AllottedGeometry,
        const double InCurrentTime,
		const float InDeltaTime) override;

    // Override AddPin to add pins to the right container
    virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

    // Override CreatePinWidgets to generate pins from GraphNode
    virtual void CreatePinWidgets() override;

    virtual EVisibility GetValidationVisibility(const FString Icon) const;

    bool bIsSelected;

    TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* Pin) const override;
private:

    bool bBoundToSelectionChange;

    UDialogEventNodeBase* EventNode;

    TSharedPtr<STextBlock> DialogLineTextWidget;

    TArray<TSharedRef<SGraphPin>> LocalPinWidgets;
};
