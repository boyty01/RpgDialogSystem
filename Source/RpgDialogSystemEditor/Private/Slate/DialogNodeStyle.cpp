// Copyright DMTesseracT LTd. All rights reserved.


#include "Slate/DialogNodeStyle.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Pins/SDialogTopConnectPin.h"
#include "Pins/SDialogBottomConnectPin.h"
#include "EditorStyleSet.h"
#include "Nodes/QuestConditionNode.h"
#include "Nodes/LocalStateConditionNode.h"
#include "Nodes/InventoryConditionNode.h"
#include "Nodes/DialogStateEventNode.h"
#include "Nodes/DialogInventoryEventNode.h"
#include "Nodes/DialogQuestEventNode.h"
#include "Slate/DialogConditionNodeStyle.h"
#include "Slate/DialogEventNodeStyle.h"
#include "Factory/DialogNodeFactory.h"
#include "NodeFactory.h"
#include "RpgDialogSlateStyle.h"
#include "Styling/SlateStyle.h"
#include "SGraphPanel.h"

void SDialogNodeStyle::Construct(const FArguments& InArgs)
{
    DialogNode = InArgs._GraphNode;
    check(DialogNode);

    GraphNode = InArgs._GraphNode;

    this->SetCursor(EMouseCursor::CardinalCross);
 //  UpdateGraphNode();
}

void SDialogNodeStyle::SetOwner(const TSharedRef<SGraphPanel>& InOwnerPanel)
{
    SGraphNode::SetOwner(InOwnerPanel);
   // SetOwner(InOwnerPanel);

    UpdateGraphNode();

    //for (auto& ChildWidget : SubNodes)
    //{
    //    if (ChildWidget.IsValid())
    //    {
    //        ChildWidget->SetOwner(InOwnerPanel);
    //        InOwnerPanel->AttachGraphEvents(ChildWidget);
    //    }
    //}

}

FSlateColor SDialogNodeStyle::GetBorderBackgroundColor() const
{

    return FLinearColor(0.025f, 0.025f, .025f, 1.f);
//    return FSlateColor(FColor(0,0,0));
}

FSlateColor SDialogNodeStyle::GetBackgroundColor() const
{    
    return FColor(108, 117, 107);
}

FSlateColor SDialogNodeStyle::GetTitleBackgroundColor() const
{
    return FColor(FColor::FromHex("#6EC1E4"));
  //  return FColor(169, 211, 255);
}

FSlateColor SDialogNodeStyle::GetAltBackgroundColor() const
{
    if (DialogNode->ConfigurationState == EDialogGraphConfigurationState::Error)
        return FLinearColor(0.8f, 0.f, 0.f, 1.f);

    if (DialogNode->ConfigurationState == EDialogGraphConfigurationState::Warning)
        return FLinearColor(1.f, 0.65f, 0.f, 1.f);

    return FColor::FromHex("#6EC1E4");
        
}

FSlateColor SDialogNodeStyle::GetAltBorderBackgroundColor() const
{
    return FColor(150, 197, 247);
}

FSlateColor SDialogNodeStyle::GetAltTitleBackgroundColor() const
{
    return FColor(169, 211, 255);
}

void SDialogNodeStyle::UpdateGraphNode()
{
   // SGraphNode::UpdateGraphNode();

    LocalPinWidgets.Empty();
    InputPins.Empty();
    OutputPins.Empty();

  //  SubNodes.Empty();
    if (ConditionWidgets)
    {
        ConditionWidgets->ClearChildren();
    }

    ConditionWidgets = SNew(SVerticalBox);
    EventWidgets = SNew(SVerticalBox);
    TopNodeBox = SNew(SHorizontalBox);
    BottomNodeBox = SNew(SHorizontalBox);
    LeftNodeBox = SNew(SVerticalBox);
    RightNodeBox = SNew(SVerticalBox);


    BuildSubnodeWidgets();

    DialogNode->ValidateNodeConfiguration();

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
                [

                    SNew(SBorder)
                        .BorderImage(FAppStyle::Get().GetBrush("Graph.Node.TintedBody"))
                        .BorderBackgroundColor(GetAltBackgroundColor())
                        .Padding(4)
                        [
                            SNew(SBorder)

                                .BorderImage(FEditorStyle::Get().GetBrush("Graph.Node.TintedBody"))                               
                                .BorderBackgroundColor(GetBorderBackgroundColor())
                                .Padding(2)
                                [
                                    SNew(SVerticalBox)

                                        // ───── Top Pins: InDialog + InChoice ─────
                                        + SVerticalBox::Slot()
                                        .AutoHeight()
                                        .HAlign(HAlign_Center)
                                        [

                                            TopNodeBox->AsShared()

                                        ]

                                        // ───── Dialog Title Info ─────
                                        + SVerticalBox::Slot()
                                        .MinHeight(30.f)
										.AutoHeight()
                                        .Padding(20, 10)
                                        [
                                            SNew(SBorder)
                                                .BorderImage(FAppStyle::GetBrush("Graph.Node.TintedBody"))
                                                .BorderBackgroundColor(GetAltBackgroundColor())
                                                .HAlign(HAlign_Fill)
                                                .VAlign(VAlign_Center)
                                                .Padding(2)
                                                [
                                                    SNew(SBorder)
                                                        .BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))
                                                        .BorderBackgroundColor(GetTitleBackgroundColor())
                                                        .HAlign(HAlign_Fill)
                                                        .VAlign(VAlign_Center)
                                                        .Padding(2,5)
                                                        [
                                                            SNew(STextBlock)
                                                                .Justification(ETextJustify::Center)
																.AutoWrapText(true)
                                                                .Text(FText::Format(
                                                                    FText::FromString("{0}"),
                                                                    DialogNode->GetNodeTitle(ENodeTitleType::FullTitle)
                                                                ))
                                                                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                                                        ]
                                                ]
                                        ]

                                        // Condition subnodes
                                        + SVerticalBox::Slot()
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

                                        +SVerticalBox::Slot()
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
                                       

                                        // ───── Expandable Attributes Section ─────
                                        + SVerticalBox::Slot()
                                        .AutoHeight()
                                        .Padding(4, 6)
                                        [
                                            SNew(SVerticalBox)

                                      

                                                //Entry Dialog Text
                                                + SVerticalBox::Slot()
                                                .AutoHeight()
                                                .Padding(0, 2)
                                                [
                                                    SNew(STextBlock)
                                                        .Text(FText::FromString("Entry Dialog"))
                                                        .Visibility(DialogNode->bShowAttributesOnNode && DialogNode->bEntryPoint
                                                            ? EVisibility::Visible
                                                            : EVisibility::Collapsed)
                                                ]

                                                // Start Priority value
                                                + SVerticalBox::Slot()
                                                .AutoHeight()
                                                .Padding(0, 2)
                                                [
                                                    SNew(STextBlock)
                                                        .Text(FText::FromString(FString::Format(TEXT("Start Priority: {0}"), { FString::FromInt(DialogNode->StartPriority) })))
                                                        .Visibility(DialogNode->bShowAttributesOnNode && DialogNode->bEntryPoint ? EVisibility::Visible : EVisibility::Collapsed)
                                                ]

                                                + SVerticalBox::Slot()
                                                .AutoHeight()
                                                .Padding(0, 2)
                                                [
                                                    SNew(STextBlock)
                                                        .Text(FText::FromString(FString::Format(TEXT("Level Sequence: {0}"), { DialogNode->LevelSequenceToPlay.IsNull() ? TEXT("None") : *DialogNode->LevelSequenceToPlay.GetAssetName() })))
                                                        .Visibility(DialogNode->bShowAttributesOnNode ? EVisibility::Visible : EVisibility::Collapsed)
                                                ]

                                                + SVerticalBox::Slot()
                                                .AutoHeight()
                                                .Padding(0, 8)
                                                [
                                                    SNew(STextBlock)
                                                        .Text(FText::Format(
                                                            FText::FromString("Dialog:\n{0}"), 
                                                            FText::Join(FText::FromString("\n"),DialogNode->Subtitles) )
                                                        )
                                                        .Visibility(DialogNode->bShowAttributesOnNode ? EVisibility::Visible : EVisibility::Collapsed)
                                                ]
                                        ]

                                        // ───── Expand/Collapse Button ─────
                                        + SVerticalBox::Slot()
                                        .AutoHeight()
                                        .Padding(0, 4)
                                        [
                                            SNew(SButton)
                                                .Text(DialogNode->bShowAttributesOnNode ? FText::FromString("Hide Attributes") : FText::FromString("Show Attributes"))
                                                .HAlign(HAlign_Center)
                                                .ToolTipText(DialogNode->bShowAttributesOnNode ? FText::FromString("Collapse") : FText::FromString("Expand"))
                                                .OnClicked(this, &SDialogNodeStyle::OnExpandClicked)
                                                .ButtonStyle(FAppStyle::Get(), "SimpleButton")
                                                .ContentPadding(FMargin(2))
                                        ]

                                        // ───── Bottom Output Pins ─────
                                        + SVerticalBox::Slot()
                                        .AutoHeight()
                                        .HAlign(HAlign_Center)
                                        .Padding(0,0)
                                        [
                                            SNew(SHorizontalBox)
                                                + SHorizontalBox::Slot()
                                                .MinWidth(15.f)
                                                .HAlign(HAlign_Center)
                                                .Padding(5, 2)
                                                [
                                                    BottomNodeBox->AsShared()
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
                    .ToolTipText(FText::FromString(DialogNode->ErrorMsg))
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
                    .ToolTipText(FText::FromString(DialogNode->ErrorMsg))
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
                            .Image(FRpgDialogSlateStyle::Get().GetBrush("RpgDialog.Speech"))  // speech icon
                            .DesiredSizeOverride(FVector2D(24.f, 24.f))
                            .ColorAndOpacity(FLinearColor::White)
                            .Visibility(EVisibility::Visible)
                            .ToolTipText(FText::FromString(DialogNode->ErrorMsg))
                    ]
            ]
        ];


    
    CreatePinWidgets();

}


EVisibility SDialogNodeStyle::GetValidationVisibility(const FString Icon) const
{
    if (DialogNode->ConfigurationState == EDialogGraphConfigurationState::Error && Icon == "Error")
        return EVisibility::Visible;
    if (DialogNode->ConfigurationState == EDialogGraphConfigurationState::Warning && Icon == "Warning")
        return EVisibility::Visible;
    return EVisibility::Collapsed;
}

void SDialogNodeStyle::CreatePinWidgets()
{
    UEdGraphNode* Node = GraphNode;

    for (UEdGraphPin* Pin : GraphNode->Pins)
    {
        if (!Pin || Pin->bHidden)
            continue;

        TSharedPtr<SGraphPin> NewPin = CreatePinWidget(Pin);
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
                    .MinWidth(15.f)
                    .Padding(4, 0)
                    [
                        SNew(SVerticalBox)
                            + SVerticalBox::Slot()
                            .AutoHeight()
                            [
                                NewPin.ToSharedRef()
                            ]
                    ];
            }
            else
            {
                RightNodeBox->AddSlot()
                    .AutoHeight()
                    .Padding(2)
                    [
                        NewPin.ToSharedRef()
                    ];
            }

        }
    }

            
}

void SDialogNodeStyle::BuildSubnodeWidgets()
{    
    if (DialogNode)
    {
        for (auto& Node : DialogNode->ChildNodes)
        {
            if (UDialogConditionsNode* ConNode = Cast<UDialogConditionsNode>(Node)) BuildConditionNode(ConNode);
            if (UDialogEventNodeBase* EventNode = Cast<UDialogEventNodeBase>(Node)) BuildEventNode(EventNode);
        }
    }

}

void SDialogNodeStyle::BuildConditionNode(UDialogConditionsNode* Node)
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
        .Padding(2,2)
        [
            SubNode.ToSharedRef()
        ];

}

void SDialogNodeStyle::BuildEventNode(UDialogEventNodeBase* Node)
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

TSharedPtr<SGraphPin> SDialogNodeStyle::CreatePinWidget(UEdGraphPin* Pin) const
{
    if (Pin->Direction == EEdGraphPinDirection::EGPD_Input && (Pin->PinName == UDialogGraphNode::InputPin_InDialog_Name || Pin->PinName == UDialogGraphNode::InputPin_InPlayerChoice_Name))
    {
        return SNew(SDialogTopConnectPin, Pin);
    }

    if (Pin->Direction == EEdGraphPinDirection::EGPD_Output && (Pin->PinName == UDialogGraphNode::OutputPin_OutDialog_Name || Pin->PinName == UDialogGraphNode::OutputPin_OutPlayerChoice_Name))
    {
        return SNew(SDialogBottomConnectPin, Pin);
    } 

    return SNew(SGraphPin, Pin);
}

FReply SDialogNodeStyle::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (UDialogGraphNode* Node = Cast<UDialogGraphNode>(GraphNode))
    {
        Node->OnNodeMoved();
    }
    return SGraphNode::OnMouseButtonUp(MyGeometry, MouseEvent);; // Release capture after handling
}

void SDialogNodeStyle::OnSubnodeSelected(UEdGraphNode* SubNode)
{
    if (TSharedPtr<SGraphPanel> OwnerPanel = GetOwnerPanel())
    {
     //   FGraphPanelSelectionSet Selection = {SubNode };
    //    OwnerPanel->SelectionManager.ClickedOnNode(SubNode);

    }
}



FReply SDialogNodeStyle::OnExpandClicked()
{
    DialogNode->bShowAttributesOnNode = !DialogNode->bShowAttributesOnNode;

    if (DialogLineTextWidget.IsValid())
    {
        DialogLineTextWidget->SetVisibility(DialogNode->bShowAttributesOnNode ? EVisibility::Visible : EVisibility::Collapsed);
    }
    UpdateGraphNode();
    return FReply::Handled();
}