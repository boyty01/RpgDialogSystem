// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ConnectionDrawingPolicy.h"

class FDialogGraphConnectionDrawPolicy : public FConnectionDrawingPolicy
{
public:

    
    FDialogGraphConnectionDrawPolicy(
        int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor,
        const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
        UEdGraph* InGraphObj)
        : FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements)
    {
      
    }
    virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params) override;

    TArray<FVector2D> CalculatePolyLinePath(const FVector2D& StartPoint,
        const FVector2D& EndPoint,
        EEdGraphPinDirection StartDirection,
        const FName& StartSubCategory,
        const FName& EndSubCategory,
        int32 ConnectionIndex = 1,
        int32 TotalConnections = 1);

    TArray<FVector2D> CalculateManhattanPath(const FVector2D& StartPoint, const FVector2D& EndPoint, EEdGraphPinDirection StartDirection, const FName& StartSubCategory,
        const FName& EndSubCategory);
    virtual void DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom, const FConnectionParams& Params) override;
    FVector2D GetConnectorCenterPoint(const FGeometry& PinGeom, EEdGraphPinDirection PinDir);
};