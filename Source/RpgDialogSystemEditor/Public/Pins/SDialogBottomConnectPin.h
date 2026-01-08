// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"
#include "Input/CursorReply.h" // For EMouseCursor::Type

class UDialogGraph;
class UDialogGraphSchema;
/**
 * 
 */
class RPGDIALOGSYSTEMEDITOR_API SDialogBottomConnectPin : public SGraphPin
{
public:
    SLATE_BEGIN_ARGS(SDialogBottomConnectPin) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

    FSlateColor PinColor;

    /*Cursor is hovered over the pin */
    bool bIsMouseOverPin;

    UDialogGraph* EdGraph;
    const UDialogGraphSchema* Schema;

    // Override the method that builds the pin content
   virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
   virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
   virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
   TOptional<EMouseCursor::Type> GetPinCursor() const;

protected:
    virtual const FSlateBrush* GetPinIcon() const override;
    virtual FSlateColor GetPinColor() const override;


    void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    void TrySetGraphAndSchema();

    /* Polled on tick when drag operations are active.*/
    bool bIsActiveDrag;
    bool bIsCompatibleWithCurrentDrag;
    bool bIsPinSourceOfDrag;


    TSharedRef<FDragDropOperation> SpawnPinDragEvent(const TSharedRef<SGraphPanel>& InGraphPanel, const TArray<TSharedRef<SGraphPin>>& InStartingPins) override;

};
