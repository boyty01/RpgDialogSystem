// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "SGraphPin.h"
#include "Nodes/DialogConditionsNode.h"


/**
 * 
 */
class RPGDIALOGSYSTEMEDITOR_API SDialogConditionsNodeStyle : public SGraphNode
{
public:
    SLATE_BEGIN_ARGS(SDialogConditionsNodeStyle) {}
        SLATE_ARGUMENT(UDialogConditionsNode*, GraphNode)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FSlateColor GetBorderBackgroundColor() const;
    virtual FSlateColor GetBackgroundColor() const { return FLinearColor::Green; };
    virtual FSlateColor GetTitleBackgroundColor() const;     
    virtual FSlateColor GetAltBackgroundColor() const;

    virtual EVisibility GetValidationVisibility(const FString Icon) const;

    bool bIsMisconfigured{ false };

    // Override UpdateGraphNode to rebuild the node UI
    virtual void UpdateGraphNode() override;

    // Override AddPin to add pins to the right container
    virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

    // Override CreatePinWidgets to generate pins from GraphNode
    virtual void CreatePinWidgets() override;


    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* Pin) const override;

protected:



private:

	void ClearValidationErrors();
	void ReportValidationError(const FString& ErrorMessage);

    UDialogConditionsNode* ConditionsNode;

    TSharedPtr<STextBlock> DialogLineTextWidget;

    TArray<TSharedRef<SGraphPin>> LocalPinWidgets;
    

    bool bIsSelected;

};
