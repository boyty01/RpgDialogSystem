// Copyright DMTesseracT LTd. All rights reserved.


#include "DialogGraphConnectionDrawPolicy.h"
#include "Slate.h"
#include "SlateBasics.h"
#include "Nodes/DialogGraphNode.h"
#include "Nodes/PlayerChoiceNode.h"

enum EDialogPinSide
{
    Left,
    Right,
    Top,
    Bottom
};

void FDialogGraphConnectionDrawPolicy::DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom, const FConnectionParams& Params)
{
    /*
    //@TODO: These values should be pushed into the Slate style, they are compensating for a bit of
    // empty space inside of the pin brush images.
    const float StartFudgeX = 14.0f * ZoomFactor;
    const float EndFudgeX = 20.0f * ZoomFactor;


    UEdGraphPin* InputPin = Params.AssociatedPin1->Direction == EEdGraphPinDirection::EGPD_Input ? Params.AssociatedPin1 : Params.AssociatedPin2;
    UEdGraphPin* OutputPin = Params.AssociatedPin1->Direction == EEdGraphPinDirection::EGPD_Input ? Params.AssociatedPin2 : Params.AssociatedPin1;

   const FVector2D StartPoint = InputPin->PinName.ToString().Contains("Condition") || InputPin->PinName.ToString().Contains("Event") ?
       FGeometryHelper::VerticalMiddleRightOf(StartGeom) - FVector2D(StartFudgeX, 0.0f) :
       FGeometryHelper::CenterOf(StartGeom);

   const FVector2D EndPoint = OutputPin->PinName.ToString().Contains("Condition") || InputPin->PinName.ToString().Contains("Event") ?
       FGeometryHelper::VerticalMiddleLeftOf(EndGeom) - FVector2D(ArrowRadius.X - EndFudgeX, 0) :
       FGeometryHelper::CenterOf(EndGeom);


    TArray<FVector2D> LinePoints;
    LinePoints.Add(StartPoint);
    LinePoints.Add(EndPoint);

    FSlateDrawElement::MakeLines(
        DrawElementsList,     // inherited from base class
        WireLayerID,            // inherited
        FPaintGeometry(),    // basic paint geometry
        LinePoints,
        ESlateDrawEffect::None,
        Params.WireColor,
        true,
        Params.WireThickness
    );*/

    const float StartFudgeX = 14.0f * ZoomFactor;
    const float EndFudgeX = 20.0f * ZoomFactor;

    UEdGraphPin* InputPin = Params.AssociatedPin1->Direction == EEdGraphPinDirection::EGPD_Input ? Params.AssociatedPin1 : Params.AssociatedPin2;
    UEdGraphPin* OutputPin = Params.AssociatedPin1->Direction == EEdGraphPinDirection::EGPD_Input ? Params.AssociatedPin2 : Params.AssociatedPin1;

    const FVector2D StartPoint = InputPin->PinName.ToString().Contains("Condition") || InputPin->PinName.ToString().Contains("Event") ?
        FGeometryHelper::VerticalMiddleRightOf(StartGeom) - FVector2D(StartFudgeX, 0.0f) :
        FGeometryHelper::CenterOf(StartGeom);

    const FVector2D EndPoint = OutputPin->PinName.ToString().Contains("Condition") || InputPin->PinName.ToString().Contains("Event") ?
        FGeometryHelper::VerticalMiddleLeftOf(EndGeom) - FVector2D(ArrowRadius.X - EndFudgeX, 0) :
        FGeometryHelper::CenterOf(EndGeom);

    // Calculate Manhattan path with three turns
    TArray<FVector2D> LinePoints = CalculateManhattanPath(StartPoint, EndPoint, InputPin->Direction, InputPin->PinType.PinSubCategory, OutputPin->PinType.PinSubCategory);

   // TArray<FVector2D> LinePoints = CalculatePolyLinePath(StartPoint, EndPoint, InputPin->Direction, OutputPin->PinType.PinSubCategory, InputPin->PinType.PinSubCategory);


    // Draw the orthogonal lines
    FSlateDrawElement::MakeLines(
        DrawElementsList,
        WireLayerID,
        FPaintGeometry(),
        LinePoints,
        ESlateDrawEffect::None,
        Params.WireColor,
        true,
        Params.WireThickness
    );

    /*
    // Draw arrow if needed
    if (Params.bDrawArrow)
    {
        FVector2D ArrowStart = LinePoints[LinePoints.Num() - 2]; // Second-to-last point
        FVector2D ArrowEnd = LinePoints[LinePoints.Num() - 1];   // Last point
        FSlateDrawElement::MakeArrow(
            DrawElementsList,
            WireLayerID,
            FPaintGeometry(),
            ArrowStart,
            ArrowEnd,
            WireColor,
            ArrowRadius
        );
    }
    */

}
void FDialogGraphConnectionDrawPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params)
{
    FConnectionDrawingPolicy::DetermineWiringStyle(OutputPin, InputPin, Params);

    UDialogEdGraphNodeBase* GraphNode = Cast<UDialogEdGraphNodeBase>(OutputPin->GetOwningNode());
   
    if (GraphNode)
    {
       Params.WireColor = GraphNode->OutPinColour;
       Params.WireThickness = GraphNode->OutPinLineWidth;
       return;
    }

    Params.WireColor = FLinearColor::White;
    Params.WireThickness = 1.f;
    return;
}

TArray<FVector2D> FDialogGraphConnectionDrawPolicy::CalculatePolyLinePath(const FVector2D& StartPoint, const FVector2D& EndPoint, EEdGraphPinDirection StartDirection, const FName& StartSubCategory, const FName& EndSubCategory, int32 ConnectionIndex, int32 TotalConnections)
{
    auto GetVerticalStub = [](const FVector2D& Point, const FName& SubCategory, float StubLength)
        {
            if (SubCategory == "Top")
            {
                return Point + FVector2D(0.f, -StubLength);
            }
            else if (SubCategory == "Bottom")
            {
                return Point + FVector2D(0.f, StubLength);
            }
            else
            {
                // Default to vertical stub if direction isn't top/bottom
                return Point + FVector2D(0.f, StubLength);
            }
        };

    const float StubLength = 20.0f * ZoomFactor;
    const float OffsetSpacing = 15.0f * ZoomFactor;

    FVector2D StartStub = GetVerticalStub(StartPoint, StartSubCategory, StubLength + 10.f);
    FVector2D EndStub = GetVerticalStub(EndPoint, EndSubCategory, StubLength);

    // Midpoint jog direction: horizontal if vertical distance is greater
    bool bPreferVertical = FMath::Abs(StartStub.Y - EndStub.Y) > FMath::Abs(StartStub.X - EndStub.X);

    float OffsetAmount = (ConnectionIndex - (TotalConnections - 1) / 2.0f) * OffsetSpacing;

    FVector2D Mid1, Mid2;
    if (bPreferVertical)
    {
        // Vertical jog: move X axis slightly
        float MidX = (StartStub.X + EndStub.X) * 0.5f + OffsetAmount;
        Mid1 = FVector2D(MidX, StartStub.Y);
        Mid2 = FVector2D(MidX, EndStub.Y);
    }
    else
    {
        // Horizontal jog: move Y axis slightly
        float MidY = (StartStub.Y + EndStub.Y) * 0.5f + OffsetAmount;
        Mid1 = FVector2D(StartStub.X, MidY);
        Mid2 = FVector2D(EndStub.X, MidY);
    }

    TArray<FVector2D> Waypoints;
    Waypoints.Add(StartPoint);
    Waypoints.Add(StartStub);
    Waypoints.Add(Mid1);
    Waypoints.Add(Mid2);
    Waypoints.Add(EndStub);
    Waypoints.Add(EndPoint);

    return Waypoints;
}
TArray<FVector2D> FDialogGraphConnectionDrawPolicy::CalculateManhattanPath(
    const FVector2D& StartPoint,
    const FVector2D& EndPoint,
    EEdGraphPinDirection StartDirection,
    const FName& StartSubCategory,
    const FName& EndSubCategory)
{
    auto ParsePinSideFromSubCategory = [](const FName& SubCategory) -> EDialogPinSide
        {
            if (SubCategory == "Left") return EDialogPinSide::Left;
            if (SubCategory == "Right") return EDialogPinSide::Right;
            if (SubCategory == "Top") return EDialogPinSide::Top;
            if (SubCategory == "Bottom") return EDialogPinSide::Bottom;
            return EDialogPinSide::Right;
        };

    auto GetStubOffset = [](EDialogPinSide Side, float Length) -> FVector2D
        {
            switch (Side)
            {
            case EDialogPinSide::Left:   return FVector2D(-Length, 0.f);
            case EDialogPinSide::Right:  return FVector2D(Length, 0.f);
            case EDialogPinSide::Top:    return FVector2D(0.f, -Length);
            case EDialogPinSide::Bottom: return FVector2D(0.f, Length);
            default:                     return FVector2D(Length, 0.f);
            }
        };

    const float StubLength = 40.0f * ZoomFactor;

    EDialogPinSide StartSide = ParsePinSideFromSubCategory(StartSubCategory);
    EDialogPinSide EndSide = ParsePinSideFromSubCategory(EndSubCategory);

    FVector2D StartStub = StartPoint + GetStubOffset(StartSide, StubLength);
    FVector2D EndStub = EndPoint + GetStubOffset(EndSide, StubLength);

    TArray<FVector2D> P;
    P.Add(StartPoint);
    P.Add(StartStub);

    //
    // 🔥 FORCE AXIS FOR FIRST SEGMENT
    //
    if (StartSide == EDialogPinSide::Top || StartSide == EDialogPinSide::Bottom)
    {
        // Vertical-first routing
        float MidY = (StartStub.Y + EndStub.Y) * 0.5f;
        P.Add(FVector2D(StartStub.X, MidY));
        P.Add(FVector2D(EndStub.X, MidY));
    }
    else
    {
        // Horizontal-first routing
        float MidX = (StartStub.X + EndStub.X) * 0.5f;
        P.Add(FVector2D(MidX, StartStub.Y));
        P.Add(FVector2D(MidX, EndStub.Y));
    }

    P.Add(EndStub);
    P.Add(EndPoint);

    return P;
    /*
    TArray<FVector2D> Waypoints;
    Waypoints.Add(StartPoint);

    // Calculate X and Y differences to determine orientation
    float DeltaX = FMath::Abs(EndPoint.X - StartPoint.X);
    float DeltaY = FMath::Abs(EndPoint.Y - StartPoint.Y);
    bool bIsHorizontal = DeltaX > DeltaY; // Horizontal if X difference is larger

    if (bIsHorizontal)
    {
        // Horizontal connection: prioritize horizontal movement
        float MidX = (StartPoint.X + EndPoint.X) * 0.5f; // Midpoint X for vertical segment
        if (StartDirection == EEdGraphPinDirection::EGPD_Output)
        {
            // Output pin (right side): extend horizontally, then vertical
            FVector2D Intermediate1(MidX, StartPoint.Y); // Horizontal to midpoint X
            FVector2D Intermediate2(MidX, EndPoint.Y);   // Vertical to end's Y
            Waypoints.Add(Intermediate1);
            Waypoints.Add(Intermediate2);
        }
        else
        {
            // Input pin (left side): extend horizontally, then vertical
            FVector2D Intermediate1(MidX, StartPoint.Y); // Horizontal to midpoint X
            FVector2D Intermediate2(MidX, EndPoint.Y);   // Vertical to end's Y
            Waypoints.Add(Intermediate1);
            Waypoints.Add(Intermediate2);
        }
    }
    else
    {
        // Vertical connection: keep existing U-shape
        float MidY = (StartPoint.Y + EndPoint.Y) * 0.5f + 20.0f * ZoomFactor; // Offset for clarity
        if (StartDirection == EEdGraphPinDirection::EGPD_Output)
        {
            FVector2D Intermediate1(StartPoint.X, MidY); // Vertical to midpoint Y
            FVector2D Intermediate2(EndPoint.X, MidY);   // Horizontal to end's X
            Waypoints.Add(Intermediate1);
            Waypoints.Add(Intermediate2);
        }
        else
        {
            FVector2D Intermediate1(StartPoint.X, MidY); // Vertical to midpoint Y
            FVector2D Intermediate2(EndPoint.X, MidY);   // Horizontal to end's X
            Waypoints.Add(Intermediate1);
            Waypoints.Add(Intermediate2);
        }
    }
    Waypoints.Add(EndPoint);

    return Waypoints;
    */
}


FVector2D FDialogGraphConnectionDrawPolicy::GetConnectorCenterPoint(const FGeometry& PinGeom, EEdGraphPinDirection PinDir)
{
    const FVector2D LocalSize = PinGeom.GetLocalSize();


    // Try to return the side center point, avoiding any label space
    FVector2D LocalPinPos;

    if (PinDir == EGPD_Input)
    {
        // Left edge center
      //  LocalPinPos = FVector2D(PinGeom.AbsolutePosition.X + 4.0f, PinGeom.AbsolutePosition.Y - 4.0f);
         LocalPinPos = FVector2D(0.0f, LocalSize.Y * 0.5f);
    }
    else // EGPD_Output
    {
        // Right edge center
        // LocalPinPos = FVector2D(PinGeom.AbsolutePosition.X + PinGeom.GetDrawSize().X - 4.0f, PinGeom.AbsolutePosition.Y);
        LocalPinPos = FVector2D(LocalSize.X, LocalSize.Y * 0.5f);
    }

    return PinGeom.LocalToAbsolute(LocalPinPos);
}
