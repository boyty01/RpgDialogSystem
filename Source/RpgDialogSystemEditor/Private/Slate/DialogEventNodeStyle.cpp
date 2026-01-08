// Copyright DMTesseracT LTd. All rights reserved.


#include "Slate/DialogEventNodeStyle.h"
#include "SGraphPanel.h"

void SDialogEventNodeStyle::Construct(const FArguments& InArgs)
{
    EventNode = InArgs._GraphNode;
    check(EventNode);

    GraphNode = InArgs._GraphNode;

    this->SetCursor(EMouseCursor::Default);
   // UpdateGraphNode();
}

FReply SDialogEventNodeStyle::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        if (GraphNode)
        {
            if (GetOwnerPanel())
            {
                GetOwnerPanel()->SelectionManager.ClearSelectionSet();
                GetOwnerPanel()->SelectionManager.SetNodeSelection(GraphNode, true);
                GraphNode->GetGraph()->NotifyGraphChanged();
            }

        }

        return FReply::Handled();
    }

    return SGraphNode::OnMouseButtonDown(MyGeometry, MouseEvent);
}

void SDialogEventNodeStyle::UpdateGraphNode()
{
    LocalPinWidgets.Empty();

    // Clear previous widgets


    InputPins.Empty();
    OutputPins.Empty();

    LeftNodeBox = SNew(SVerticalBox);
    RightNodeBox = SNew(SVerticalBox);

    EventNode->ValidateNodeConfiguration();

    this->ContentScale.Bind(this, &SGraphNode::GetContentScale);

    this->GetOrAddSlot(ENodeZone::Center)
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SOverlay)
                + SOverlay::Slot()
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)
                [
            SNew(SBox)
                .MinDesiredWidth(300.f)
                .Padding(2)
                [
                    SNew(SBorder)
                        .BorderImage(FAppStyle::GetBrush("Graph.Node.TintedBody"))
                        .BorderBackgroundColor(bIsSelected ? GetBorderBackgroundColor() : FLinearColor(0.f, 0.f, 1.f, 0.0f))
                        .Padding(2)
                        [
                            SNew(SVerticalBox)                               
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                .HAlign(HAlign_Fill)
                                [
                                    SNew(SBorder)
                                        .BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))
                                        .BorderBackgroundColor(FColor::FromHex("#F1C40F"))
                                        .HAlign(HAlign_Fill)
                                        .Padding(4)
                                        [
                                            SNew(STextBlock)
                                                .Text(FText::Format(FText::FromString("{0}"), EventNode->GetNodeTitle(ENodeTitleType::FullTitle)))
                                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
                                        ]
                                ]
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                .VAlign(VAlign_Top)
                                [
                                    SNew(SHorizontalBox)
    
                                        // Right pins vertical stack
                                        + SHorizontalBox::Slot()
                                        .HAlign(HAlign_Right)
                                        .VAlign(VAlign_Bottom)
                                        [
                                            RightNodeBox.ToSharedRef()
                                        ]
                                ]
                        ]
                ]
            ]
            + SOverlay::Slot()
                .HAlign(HAlign_Left)
                .VAlign(VAlign_Center)
                .Padding(-15.0f)
                [
                    SNew(SImage)
                        .Image(FAppStyle::GetBrush("SceneOutliner.NewFolderIcon"))  // Event icon
                        .ColorAndOpacity(FLinearColor::White)
                        .Visibility(EVisibility::Visible)
                ]
                + SOverlay::Slot()
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Center)
                .Padding(-15.0f)
                [
                    SNew(SImage)
                        .Image(FAppStyle::GetBrush("MessageLog.Warning"))  // X / Warning icon
                        .ColorAndOpacity(FLinearColor::Yellow)
                        .Visibility(GetValidationVisibility("Warning"))
                        .ToolTipText(FText::FromString(EventNode->ErrorMsg))
                ]
            + SOverlay::Slot()
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Center)
                .Padding(-15.0f)
                [
                    SNew(SImage)
                        .Image(FAppStyle::GetBrush("Icons.Error"))  // X / error icon
                        .ColorAndOpacity(FLinearColor::Red)
                        .Visibility(GetValidationVisibility("Error"))
                        .ToolTipText(FText::FromString(EventNode->ErrorMsg))
                ]
        ];

    CreatePinWidgets();

}

EVisibility SDialogEventNodeStyle::GetValidationVisibility(const FString Icon) const
{
    if (EventNode->ConfigurationState == EDialogGraphConfigurationState::Error && Icon == "Error")
        return EVisibility::Visible;

    if (EventNode->ConfigurationState == EDialogGraphConfigurationState::Warning && Icon == "Warning")
        return EVisibility::Visible;

    return EVisibility::Collapsed;
}

void SDialogEventNodeStyle::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SGraphNode::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    TSharedPtr<SGraphPanel> Panel = GetOwnerPanel();
    if (!Panel.IsValid()) return;

    const bool bCurrentlySelected = Panel->SelectionManager.IsNodeSelected(EventNode);
    if (bCurrentlySelected != bIsSelected)
    {
        bIsSelected = bCurrentlySelected;
        UpdateGraphNode();
    }
}

void SDialogEventNodeStyle::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
    PinToAdd->SetOwner(SharedThis(this));

    if (PinToAdd->GetDirection() == EGPD_Input)
    {
        LeftNodeBox->AddSlot()
            .AutoHeight()
            [
                PinToAdd
            ];
        InputPins.Add(PinToAdd);
    }
    else // EGPD_Output
    {
        RightNodeBox->AddSlot()
            .HAlign(HAlign_Right)
            .AutoHeight()
            [
                PinToAdd
            ];
        OutputPins.Add(PinToAdd);
    }
}

void SDialogEventNodeStyle::CreatePinWidgets()
{
    UEdGraphNode* Node = GraphNode;
    if (!Node) return;

    for (int32 PinIdx = 0; PinIdx < Node->Pins.Num(); PinIdx++)
    {
        UEdGraphPin* MyPin = Node->Pins[PinIdx];
        if (!MyPin || MyPin->bHidden)
        {
            continue;
        }

        TSharedPtr<SGraphPin> NewPin = CreatePinWidget(MyPin);
        if (NewPin.IsValid())
        {
            AddPin(NewPin.ToSharedRef());
        }
    }
}

TSharedPtr<SGraphPin> SDialogEventNodeStyle::CreatePinWidget(UEdGraphPin* Pin) const
{
    return SNew(SGraphPin, Pin);
}
