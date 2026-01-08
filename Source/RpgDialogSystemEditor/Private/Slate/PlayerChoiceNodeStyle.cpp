// Copyright DMTesseracT LTd. All rights reserved.


#include "Slate/PlayerChoiceNodeStyle.h"
#include "Pins/SDialogTopConnectPin.h"
#include "Pins/SDialogBottomConnectPin.h"
#include "NodeFactory.h"
#include "Styling/SlateStyle.h"
#include "RpgDialogSlateStyle.h"
#include "SGraphPanel.h"
#include "SGraphNode.h"
#include "Nodes/DialogEventNodeBase.h"

void SPlayerChoiceNodeStyle::Construct(const FArguments& InArgs)
{
    ChoiceNode = InArgs._GraphNode;
    check(ChoiceNode);

    GraphNode = InArgs._GraphNode;

    this->SetCursor(EMouseCursor::CardinalCross);
    UpdateGraphNode();
    TitleBorderMargin = FMargin(0);
}

void SPlayerChoiceNodeStyle::SetOwner(const TSharedRef<SGraphPanel>& InOwnerPanel)
{
    SGraphNode::SetOwner(InOwnerPanel);
    UpdateGraphNode();
}

FSlateColor SPlayerChoiceNodeStyle::GetAltBackgroundColor() const
{
    if (ChoiceNode->ConfigurationState == EDialogGraphConfigurationState::Error)
        return FLinearColor(0.8f, 0.f, 0.f, 1.f);

    if (ChoiceNode->ConfigurationState == EDialogGraphConfigurationState::Warning)
        return FLinearColor(1.f, 0.65f, 0.f, 1.f);

    return FColor::FromHex("#2ECC71");
}

void SPlayerChoiceNodeStyle::UpdateGraphNode()
{
    LocalPinWidgets.Empty();



    InputPins.Empty();
    OutputPins.Empty();

    TopNodeBox = SNew(SHorizontalBox);
    BottomNodeBox = SNew(SHorizontalBox);
    LeftNodeBox = SNew(SVerticalBox);
    RightNodeBox = SNew(SVerticalBox);

    ChoiceNode->ValidateNodeConfiguration();
    //  SubNodes.Empty();
    if (ConditionWidgets)
    {
        ConditionWidgets->ClearChildren();
    }

    ConditionWidgets = SNew(SVerticalBox);
    EventWidgets = SNew(SVerticalBox);

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
                .MaxDesiredWidth(300.f)
                .Padding(0,0)
                [
                    SNew(SBorder)
                        .BorderImage(FAppStyle::GetBrush("Graph.Node.TintedBody"))
                        .BorderBackgroundColor(GetAltBackgroundColor())
                        .Padding(4)
                        .VAlign(VAlign_Fill)
                        [
                            SNew(SBorder)
                                .BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))
                                .BorderBackgroundColor(FColor::FromHex("#000000"))
                                .Padding(0)
                                .VAlign(VAlign_Fill)
                        [
                            SNew(SVerticalBox)
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                .Padding(0)                                
                                .VAlign(VAlign_Top)
                                [
                                    SNew(SHorizontalBox)
                                        // Left pins vertical stack
                                        + SHorizontalBox::Slot()
                                        .HAlign(HAlign_Center)
                                        .VAlign(VAlign_Top)
                                        [
                                            TopNodeBox.ToSharedRef()
                                        ]
                                ]
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                .HAlign(HAlign_Fill)
                                .Padding(20)
                                [
                                    SNew(SBorder)
                                        .BorderImage(FAppStyle::GetBrush("Graph.Node.TintedBody"))
                                        .BorderBackgroundColor(FColor::FromHex("#2ECC71"))
                                        .HAlign(HAlign_Fill)
                                        .Padding(2)
                                        [
                                            SNew(SBorder)
                                                .BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))
                                                .BorderBackgroundColor(FColor::FromHex("#2ECC71"))
                                                .HAlign(HAlign_Fill)
                                                .Padding(5)
                                                [
                                                    SNew(STextBlock)
                                                        .Justification(ETextJustify::Center)
														.AutoWrapText(true)
                                                        .Text(FText::Format(FText::FromString("{0}"), ChoiceNode->GetNodeTitle(ENodeTitleType::FullTitle)))
                                                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                                                ]
                                        ]
                                   
                                ]
                           
                                    // Condition subnodes
                                    +SVerticalBox::Slot()
                                        .AutoHeight()
                                        .Padding(20, 0)
                                        [

                                            SNew(SBorder)
                                                .BorderImage(FAppStyle::GetBrush("Graph.Node.TintedBody"))
                                                .BorderBackgroundColor(FLinearColor::Transparent)
                                                [
                                                    ConditionWidgets.ToSharedRef()
                                                ]
                                        ]

                                        + SVerticalBox::Slot()
                                        .AutoHeight()
                                        .Padding(20, 0)
                                        [
                                            SNew(SBorder)
                                                .BorderImage(FAppStyle::GetBrush("Graph.Node.TintedBody"))
                                                .BorderBackgroundColor(FLinearColor::Transparent)
                                                [
                                                    EventWidgets.ToSharedRef()
                                                ]
                                        ]
                       
                                + SVerticalBox::Slot()
                                .AutoHeight()
                                .VAlign(VAlign_Fill)
                                .Padding(0)
                                [
                                    SNew(SHorizontalBox)
    
                                        // Right pins vertical stack
                                        + SHorizontalBox::Slot()
                                        .HAlign(HAlign_Center)
                                        .VAlign(VAlign_Bottom)
                                        [
                                            RightNodeBox.ToSharedRef()
                                        ]
                                ]
                        ]
                ]
                ]
        ]
        + SOverlay::Slot()
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Top)
            .Padding(5.0f)
            [
                SNew(SBorder)
                    .BorderBackgroundColor(FLinearColor::Transparent)
                    .BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))
                    [
                        SNew(SImage)
                            .Image(FAppStyle::GetBrush("MessageLog.Warning"))  // X / Warning icon                    
                            .ColorAndOpacity(FLinearColor::Yellow)
                            .DesiredSizeOverride(FVector2D(24.f, 24.f))
                            .Visibility(GetValidationVisibility("Warning"))
                            .ToolTipText(FText::FromString(ChoiceNode->ErrorMsg))
                    ]
            ]
        + SOverlay::Slot()
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Top)
            .Padding(5.0f)
            [
                SNew(SBorder)
                    .BorderBackgroundColor(FLinearColor::Transparent)
                    .BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))

                    [
                        SNew(SImage)
                            .Image(FAppStyle::GetBrush("Icons.Error"))  // X / error icon
                            .DesiredSizeOverride(FVector2D(24.f, 24.f))
                            .ColorAndOpacity(FLinearColor::Red)
                            .Visibility(GetValidationVisibility("Error"))
                            .ToolTipText(FText::FromString(ChoiceNode->ErrorMsg))
                    ]
            ]
        + SOverlay::Slot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Top)
            .Padding(5.f)
            [
                SNew(SBorder)
                    .BorderBackgroundColor(FLinearColor::Transparent)
                    .BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))

                    [
                        SNew(SImage)
                            .Image(FRpgDialogSlateStyle::Get().GetBrush("RpgDialog.SplitPath"))  // paths icon
                            .DesiredSizeOverride(FVector2D(24.f, 24.f))
                            .ColorAndOpacity(FLinearColor::White)
                            .Visibility(EVisibility::Visible)
                    ]
            ]
        ];

    CreatePinWidgets();
    BuildSubnodeWidgets();
}

EVisibility SPlayerChoiceNodeStyle::GetValidationVisibility(const FString Icon) const
{
    if (ChoiceNode->ConfigurationState == EDialogGraphConfigurationState::Error && Icon == "Error")
        return EVisibility::Visible;
    if (ChoiceNode->ConfigurationState == EDialogGraphConfigurationState::Warning && Icon == "Warning")
        return EVisibility::Visible;
	return EVisibility::Collapsed;
}

void SPlayerChoiceNodeStyle::BuildSubnodeWidgets()
{
    if (ChoiceNode)
    {
        for (auto& Node : ChoiceNode->ChildNodes)
        {
            if (UDialogConditionsNode* ConNode = Cast<UDialogConditionsNode>(Node)) BuildConditionNode(ConNode);
            if (UDialogEventNodeBase* EventNode = Cast<UDialogEventNodeBase>(Node)) BuildEventNode(EventNode);
        }
    }

}

void SPlayerChoiceNodeStyle::BuildConditionNode(UDialogConditionsNode* Node)
{
    TSharedPtr<SGraphNode> SubNode = FNodeFactory::CreateNodeWidget(Node);
    if (OwnerGraphPanelPtr.IsValid())
    {
        if (!SubNodes.Contains(SubNode))
        {
            SubNodes.Add(SubNode);
            SubNode->SetOwner(OwnerGraphPanelPtr.Pin().ToSharedRef());
            OwnerGraphPanelPtr.Pin()->AttachGraphEvents(SubNode);
        }
        SubNode->UpdateGraphNode();
    }

    ConditionWidgets->AddSlot()
        .Padding(2)
        [
            SubNode.ToSharedRef()
        ];

}

void SPlayerChoiceNodeStyle::BuildEventNode(UDialogEventNodeBase* Node)
{
    TSharedPtr<SGraphNode> SubNode = FNodeFactory::CreateNodeWidget(Node);
    if (OwnerGraphPanelPtr.IsValid())
    {
        if (!SubNodes.Contains(SubNode))
        {
            SubNodes.Add(SubNode);
            SubNode->SetOwner(OwnerGraphPanelPtr.Pin().ToSharedRef());
            OwnerGraphPanelPtr.Pin()->AttachGraphEvents(SubNode);
        }
        SubNode->UpdateGraphNode();
    }

    EventWidgets->AddSlot()
        .Padding(2)
        [
            SubNode.ToSharedRef()
        ];
}

void SPlayerChoiceNodeStyle::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
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

void SPlayerChoiceNodeStyle::CreatePinWidgets()
{
    UEdGraphNode* Node = GraphNode;

    for (UEdGraphPin* Pin : GraphNode->Pins)
    {
        if (!Pin || Pin->bHidden)
            continue;

        TSharedPtr<SGraphPin> NewPin = CreatePinWidget(Pin);
        // AddPin(NewPin.ToSharedRef()); // still needed for management

        NewPin->SetOwner(SharedThis(this));

        if (Pin->Direction == EGPD_Input)
        {

            InputPins.Add(NewPin.ToSharedRef());
            if (Pin->PinName == UDialogGraphNode::InputPin_InDialog_Name || Pin->PinName == UDialogGraphNode::InputPin_InPlayerChoice_Name)
            {
                TopNodeBox->AddSlot()
                    .FillWidth(1.0f)
                    .Padding(1, 0)
                    [

                        NewPin.ToSharedRef()

                    ];
            }
            else
            {
                LeftNodeBox->AddSlot()
                    .AutoHeight()
                    .Padding(2)
                    [
                        NewPin.ToSharedRef()
                    ];
            }

        }
        else if (Pin->Direction == EGPD_Output)
        {
            OutputPins.Add(NewPin.ToSharedRef());
            if (Pin->PinName == UDialogGraphNode::OutputPin_OutDialog_Name || Pin->PinName == UDialogGraphNode::OutputPin_OutPlayerChoice_Name)
            {
                BottomNodeBox->AddSlot()
                    .Padding(4, 0)
                    [
                        SNew(SBox)
                            .MinDesiredWidth(150)
                            [
                                NewPin.ToSharedRef()
                            ]
                    ];
            }
            else
            {
                RightNodeBox->AddSlot()
                    .AutoHeight()
                    .Padding(0)
                    [
                        NewPin.ToSharedRef()
                    ];
            }

        }
    }

}

TSharedPtr<SGraphPin> SPlayerChoiceNodeStyle::CreatePinWidget(UEdGraphPin* Pin) const
{
    if (Pin->PinName == UPlayerChoiceNode::InputPin_InDialog_Name)
    {
        return SNew(SDialogTopConnectPin, Pin);
    }

    if (Pin->PinName == UPlayerChoiceNode::OutputPin_OutDialog_Name)
    {
        return SNew(SDialogBottomConnectPin, Pin);
    }

    return SNew(SGraphPin, Pin);
}
