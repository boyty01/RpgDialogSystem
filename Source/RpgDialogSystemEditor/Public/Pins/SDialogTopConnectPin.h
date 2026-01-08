// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"

class UDialogGraph;
class UDialogGraphSchema;
/**
 * 
 */
class RPGDIALOGSYSTEMEDITOR_API SDialogTopConnectPin : public SGraphPin
{
public:
    SLATE_BEGIN_ARGS(SDialogTopConnectPin) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

    FSlateColor PinColor;

   UDialogGraph* EdGraph;
   const UDialogGraphSchema* Schema; 

    // Override the method that builds the pin content
   virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
   virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
   virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
   virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
   virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;

protected:
    virtual const FSlateBrush* GetPinIcon() const override;
    virtual FSlateColor GetPinColor() const override;

    void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    bool bIsMouseOverPin;
    /* Polled on tick when drag operations are active.*/
    bool bIsActiveDrag;
    bool bIsCompatibleWithCurrentDrag;
    bool bIsPinSourceOfDrag;

    void TrySetGraphAndSchema();

    TSharedRef<FDragDropOperation> SpawnPinDragEvent(const TSharedRef<SGraphPanel>& InGraphPanel, const TArray< TSharedRef<SGraphPin> >& InStartingPins) override;
private:

};
