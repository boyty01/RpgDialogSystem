// Copyright DMTesseracT LTd. All rights reserved.


#include "Pins/SDialogTopConnectPin.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Brushes/SlateColorBrush.h"
#include "Graph/DialogGraph.h"
#include "Schema/DialogGraphSchema.h"

void SDialogTopConnectPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
    this->SetCursor(EMouseCursor::Crosshairs);
    bShowLabel = false;
    SetIsEditable(IsEditable);
    PinColor = FSlateColor(FLinearColor(1.f, 0.1f, 0.1f, 1.f));

   // EdGraph = Cast<UDialogGraph>(GraphPinObj->GetOwningNode()->GetGraph());
   // Schema = Cast<UDialogGraphSchema>(EdGraph->GetSchema());

    GraphPinObj = InPin;
    check(GraphPinObj != nullptr);

    SetBorderImage(FStyleDefaults::GetNoBrush());
    
    TSharedRef<SWidget> PinWidget =
        SNew(SBox)
        .HeightOverride(12)
        .MinDesiredWidth(12)
        .MaxDesiredWidth(12)
        .HAlign(HAlign_Fill)
        [
            SNew(SBorder)
                .HAlign(HAlign_Fill)
                .BorderImage(FAppStyle::Get().GetBrush("Graph.Node.TintedBody"))
                .BorderBackgroundColor(FLinearColor(0.7f, 0.7f, 0.7f))
                .Padding(1)
                [
                    SNew(SBorder)
                        .HAlign(HAlign_Fill)
                        .BorderImage(FAppStyle::GetBrush("Graph.Node.TintedBody"))
                        .BorderBackgroundColor(this, &SDialogTopConnectPin::GetPinColor)
                ]

        ];
    
    SetPinImageWidget(PinWidget);

    TSharedPtr<SVerticalBox> Content =
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Center)
        [
            PinWidget
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Center)
        .Padding(4)
        [
            SNew(STextBlock)
                .Justification(ETextJustify::Center)
                .MinDesiredWidth(100)
                .Text(FText::FromName(GraphPinObj->PinName))
                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
        ];

    SBorder::Construct(SBorder::FArguments()
        .BorderImage(FStyleDefaults::GetNoBrush())
        .BorderBackgroundColor(this, &SDialogTopConnectPin::GetHighlightColor)
        .OnMouseButtonDown(this, &SDialogTopConnectPin::OnPinMouseDown)
        [
            SNew(SBorder)
                .BorderImage(CachedImg_Pin_DiffOutline)
                .BorderBackgroundColor(this, &SDialogTopConnectPin::GetPinDiffColor)
                [
                    Content.ToSharedRef()
                ]
        ]);

    TrySetGraphAndSchema();
}

TSharedRef<SWidget> SDialogTopConnectPin::GetDefaultValueWidget()
{
    return SNullWidget::NullWidget;

}

void SDialogTopConnectPin::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    bIsMouseOverPin = true;
    PinColor = FLinearColor(0.f, 1.f, 0.f, 1.f);

    SGraphPin::OnMouseEnter(MyGeometry, MouseEvent);
}

void SDialogTopConnectPin::OnMouseLeave(const FPointerEvent& MouseEvent)
{
    bIsMouseOverPin = false;
    PinColor = FSlateColor(FLinearColor(1.f, 0.1f, 0.1f, 1.f));

    SGraphPin::OnMouseLeave(MouseEvent);
}

FReply SDialogTopConnectPin::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
   OnMouseEnter(MyGeometry, DragDropEvent);
   return SGraphPin::OnDragOver(MyGeometry, DragDropEvent);
}

void SDialogTopConnectPin::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
    SGraphPin::OnDragLeave(DragDropEvent);
    PinColor = FSlateColor(FLinearColor(1.f, 0.1f, 0.1f, 1.f));
}


const FSlateBrush* SDialogTopConnectPin::GetPinIcon() const
{
    return FAppStyle::GetBrush("NoBorder");
}

FSlateColor SDialogTopConnectPin::GetPinColor() const
{    

    bool bHasConnections = GraphPinObj->HasAnyConnections();
    // On mouse over
    if (bIsMouseOverPin && !bIsActiveDrag)
    {
        // hover with connections
        if (bHasConnections)
        {
            return  FLinearColor(1.f, 1.f, 1.f, 1.0f);
        }

        // hover no connections
        return FLinearColor(1.f, 1.f, 1.f, 1.0f);
    }


    // Drag event colouring
    if (bIsActiveDrag)
    {
        if (bIsPinSourceOfDrag)
        {
            return FLinearColor(0.f, 1.f, 1.f, 1.0f);
        }

        if (bIsCompatibleWithCurrentDrag)
        {
            // warn if compatible but target has connections (Regardless of whether it will replace)
            if (bHasConnections)
            {
                return FLinearColor::Yellow;
            }

            return FLinearColor(0.f, 1.f, 0.f, 1.f);
        }

        // not source and not compatible
        return FLinearColor::Red;
    }


    // no hover, with connections.
    if (bHasConnections)
    {
        return FLinearColor(FLinearColor(0.f, 1.f, 1.f, 1.f));
    }

    // Standard colouring

    // Nothing connected, no hover.
    return FSlateColor(FLinearColor(.6f, .6f, .6f, 1.f));
}

void SDialogTopConnectPin::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SGraphPin::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    bIsCompatibleWithCurrentDrag = false;

    if (FSlateApplication::Get().IsDragDropping())
    {
        if (!EdGraph || !Schema) TrySetGraphAndSchema();
       
        if (EdGraph && Schema)
        {
            if(UEdGraphPin* DraggingPin = EdGraph->GetDraggingPin())            
            {
                bIsActiveDrag = true;
                bIsPinSourceOfDrag = DraggingPin == GraphPinObj;
                bIsCompatibleWithCurrentDrag = Schema->ArePinsCompatible(DraggingPin, GraphPinObj);
            }
        }     
        return;
    }

    bIsActiveDrag = false;
    bIsPinSourceOfDrag = false;
    bIsCompatibleWithCurrentDrag = false;
    return;
}

void SDialogTopConnectPin::TrySetGraphAndSchema()
{
    if (GraphPinObj->GetOwningNode() && GraphPinObj->GetOwningNode()->GetGraph())
    {
        EdGraph = Cast<UDialogGraph>(GraphPinObj->GetOwningNode()->GetGraph());
        
        if (EdGraph)
        {
            Schema = Cast<UDialogGraphSchema>(EdGraph->GetSchema());
        }
    }
    
}

TSharedRef<FDragDropOperation> SDialogTopConnectPin::SpawnPinDragEvent(const TSharedRef<SGraphPanel>& InGraphPanel, const TArray<TSharedRef<SGraphPin>>& InStartingPins)
{
    if (EdGraph)
    {
        EdGraph->OnPinDragStarted(GetPinObj());
    }

    return SGraphPin::SpawnPinDragEvent(InGraphPanel, InStartingPins);    
}
