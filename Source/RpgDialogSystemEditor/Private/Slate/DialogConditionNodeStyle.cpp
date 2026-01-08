// Copyright DMTesseracT LTd. All rights reserved.


#include "Slate/DialogConditionNodeStyle.h"
#include "Misc/UObjectToken.h"
#include "SGraphPanel.h"

void SDialogConditionsNodeStyle::Construct(const FArguments& InArgs)
{
    ConditionsNode = InArgs._GraphNode;
    check(ConditionsNode);

    GraphNode = InArgs._GraphNode;
    this->SetCursor(EMouseCursor::Default);
    UpdateGraphNode();
}

FReply SDialogConditionsNodeStyle::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
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

FSlateColor SDialogConditionsNodeStyle::GetTitleBackgroundColor() const
{
    return FColor::FromHex("#1ABC9C");
  //  return FColor(169, 211, 255);
}

FSlateColor SDialogConditionsNodeStyle::GetAltBackgroundColor() const
{
    return FLinearColor(1.0f, 1.f, 1.f);
}

EVisibility SDialogConditionsNodeStyle::GetValidationVisibility(const FString Icon) const
{
    if (ConditionsNode->ConfigurationState == EDialogGraphConfigurationState::Error && Icon == "Error")
        return EVisibility::Visible;

    if(ConditionsNode->ConfigurationState == EDialogGraphConfigurationState::Warning && Icon == "Warning")
		return EVisibility::Visible;

    return EVisibility::Collapsed;
}

void SDialogConditionsNodeStyle::UpdateGraphNode()
{
    LocalPinWidgets.Empty();

    // Clear previous widgets


    InputPins.Empty();
    OutputPins.Empty();

    LeftNodeBox = SNew(SVerticalBox);
    RightNodeBox = SNew(SVerticalBox);

    ConditionsNode->ValidateNodeConfiguration();

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
                .Padding(4)
                [
                    SNew(SBorder)
                        .BorderImage(FAppStyle::GetBrush("Graph.Node.TintedBody"))
                        .BorderBackgroundColor(GetBorderBackgroundColor())
                        .Padding(2)
                        [

                            SNew(SBorder)
                                .BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))
                                .BorderBackgroundColor(GetTitleBackgroundColor())
                                .HAlign(HAlign_Fill)
                                .Padding(4)
                                [
                                    SNew(STextBlock)
										.AutoWrapText(true)
                                        .Text(FText::Format(FText::FromString("{0}"), ConditionsNode->GetNodeTitle(ENodeTitleType::FullTitle)))
                                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
                                        .ColorAndOpacity(FLinearColor(0.5f,0.5f,0.5f))
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
                        .Image(FAppStyle::GetBrush("LayerBrowser.ExploreLayerContents"))  // Condition icon
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
                        .ToolTipText(FText::FromString(ConditionsNode->ErrorMsg))
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
                        .ToolTipText(FText::FromString(ConditionsNode->ErrorMsg))
                ]
        ];

    CreatePinWidgets();

}

FSlateColor SDialogConditionsNodeStyle::GetBorderBackgroundColor() const
{ 
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

void SDialogConditionsNodeStyle::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
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

void SDialogConditionsNodeStyle::CreatePinWidgets()
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

TSharedPtr<SGraphPin> SDialogConditionsNodeStyle::CreatePinWidget(UEdGraphPin* Pin) const
{
    return SNew(SGraphPin, Pin);
}



void SDialogConditionsNodeStyle::ClearValidationErrors()
{
    ConditionsNode->ErrorMsg = "";
    ConditionsNode->ErrorType = EMessageSeverity::Info;
    ConditionsNode->bHasCompilerMessage = false;
	ConditionsNode->GetGraph()->NotifyGraphChanged();
}

void SDialogConditionsNodeStyle::ReportValidationError(const FString& ErrorMessage)
{
    ConditionsNode->ErrorMsg = ErrorMessage;
    ConditionsNode->ErrorType = EMessageSeverity::Error;
    ConditionsNode->bHasCompilerMessage = true;
	ConditionsNode->GetGraph()->NotifyGraphChanged();
}

void SDialogConditionsNodeStyle::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SGraphNode::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    TSharedPtr<SGraphPanel> Panel = GetOwnerPanel();
    if (!Panel.IsValid()) return;

    const bool bCurrentlySelected = Panel->SelectionManager.IsNodeSelected(GraphNode);
    if (bCurrentlySelected != bIsSelected)
    {
		bIsSelected = bCurrentlySelected;
        UpdateGraphNode();
    }
}
