// Copyright DMTesseracT LTd. All rights reserved.


#include "Schema/DialogGraphSchema.h"
#include "ToolMenus.h"
#include "EdGraphNode_Comment.h"
#include "Nodes/PlayerChoiceNode.h"
#include "Nodes/DialogCommentNode.h"
#include "DataAsset/DialogTree.h"


FString InDialogPinName = "InDialog";
FString OutDialogPinName = "NextDialog";
FString InPlayerChoicePinName = "InChoice";
FString OutPlayerChoicePinName = "OutChoice";

void UDialogGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{    
    TSharedPtr<FDialogGraphSchemaAction_NewDialogNode> NewNodeAction = MakeShareable(new FDialogGraphSchemaAction_NewDialogNode());
    ContextMenuBuilder.AddAction(NewNodeAction);
    ContextMenuBuilder.AddAction(MakeShareable(new FDialogGraphSchemaAction_PlayerChoice()));

    const FText CommentMenuText = FText::FromString(TEXT("Add Comment"));
    const FText CommentToolTip = FText::FromString(TEXT("Create a comment box to group nodes"));
  
    ContextMenuBuilder.AddAction(MakeShareable(new FDialogGraphSchemaAction_NewComment));
}

void UDialogGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
    Super::GetContextMenuActions(Menu, Context);

    if (!Context || !Context->Node)
        return;

    const UEdGraphNode* Node = Context->Node;
    UDialogEdGraphNodeBase* DialogNode = Cast<UDialogEdGraphNodeBase>(const_cast<UEdGraphNode*>(Node)); // yuck
    if (!DialogNode)
        return;

    if (IRpgDialogNodeInterface* AsInterface = Cast<IRpgDialogNodeInterface>(DialogNode))
    {
        if (AsInterface->SupportsConditionsSubNode())
        {
            FToolMenuSection& Section = Menu->FindOrAddSection("SubNodes|Conditions");

            Section.AddMenuEntry(
                "MyGraph_AddStateCondition",
                FText::FromString("Add State Condition"),
                FText::FromString("Adds a decorator/condition to this node"),
                FSlateIcon(),
                FExecuteAction::CreateLambda([DialogNode, this]() {
                    UEdGraph* Graph = DialogNode->GetGraph();
                    UDialogGraphSchema::AddConditionSubNode<ULocalStateConditionNode>(DialogNode, Graph);
                    })
            );

            Section.AddMenuEntry(
                "MyGraph_AddInventoryCondition",
                FText::FromString("Add Inventory Condition"),
                FText::FromString("Adds a decorator/condition to this node"),
                FSlateIcon(),
                FExecuteAction::CreateLambda([DialogNode, this]() {
                    UEdGraph* Graph = DialogNode->GetGraph();
                    UDialogGraphSchema::AddConditionSubNode<UInventoryConditionNode>(DialogNode, Graph);
                    })
            );

            Section.AddMenuEntry(
                "MyGraph_AddQuestCondition",
                FText::FromString("Add Quest Condition"),
                FText::FromString("Adds a decorator/condition to this node"),
                FSlateIcon(),
                FExecuteAction::CreateLambda([DialogNode, this]() {
                    UEdGraph* Graph = DialogNode->GetGraph();
                    UDialogGraphSchema::AddConditionSubNode<UQuestConditionNode>(DialogNode, Graph);
                    })
            );
        }

        if (AsInterface->SupportsEventsSubNode())
        {
            FToolMenuSection& Section = Menu->FindOrAddSection("SubNodes|Events");

            Section.AddMenuEntry(
                "MyGraph_AddStateEvent",
                FText::FromString("Add State Event"),
                FText::FromString("Adds a decorator/condition to this node"),
                FSlateIcon(),
                FExecuteAction::CreateLambda([DialogNode, this]() {
                    UEdGraph* Graph = DialogNode->GetGraph();
                    UDialogGraphSchema::AddEventSubNode<UDialogStateEventNode>(DialogNode, Graph);
                    })
            );

            Section.AddMenuEntry(
                "MyGraph_AddInventoryEvent",
                FText::FromString("Add Inventory Event"),
                FText::FromString("Adds a decorator/condition to this node"),
                FSlateIcon(),
                FExecuteAction::CreateLambda([DialogNode, this]() {
                    UEdGraph* Graph = DialogNode->GetGraph();
                    UDialogGraphSchema::AddEventSubNode<UDialogInventoryEventNode>(DialogNode, Graph);
                    })
            );

            Section.AddMenuEntry(
                "MyGraph_AddQuestEvent",
                FText::FromString("Add Quest Event"),
                FText::FromString("Adds a decorator/condition to this node"),
                FSlateIcon(),
                FExecuteAction::CreateLambda([DialogNode, this]() {
                    UEdGraph* Graph = DialogNode->GetGraph();
                    UDialogGraphSchema::AddEventSubNode<UDialogQuestEventNode>(DialogNode, Graph);
                    })
            );

            Section.AddMenuEntry(
                "MyGraph_AddCustomEvent",
                FText::FromString("Add Custom Script Event"),
                FText::FromString("Adds a decorator/condition to this node"),
                FSlateIcon(),
                FExecuteAction::CreateLambda([DialogNode, this]() {
                    UEdGraph* Graph = DialogNode->GetGraph();
                    UDialogGraphSchema::AddEventSubNode<UDialogCustomEventNode>(DialogNode, Graph);
                    })
            );
        }
    }
   
 
}

UDialogCommentNode* UDialogGraphSchema::CreateComment(UEdGraph* Graph, const FVector2D& Location, const FVector2D& Size, const FText& Title, FGuid NodeId, bool bIsNewNode) const
{
    UDialogCommentNode* NewComment = NewObject<UDialogCommentNode>(Graph);

    NewComment->NodeGuid = NodeId;
    NewComment->NodePosX = Location.X;
    NewComment->NodePosY = Location.Y;
	NewComment->NodeWidth = Size.X;
	NewComment->NodeHeight = Size.Y;

    NewComment->NodeComment = Title.ToString(); 
    NewComment->bColorCommentBubble = true;
    NewComment->bCommentBubbleVisible = true;
    NewComment->CommentColor = FLinearColor::Blue;

    NewComment->PostPlacedNewNode();
    NewComment->AllocateDefaultPins();

    Graph->AddNode(NewComment, true, true);
	Graph->NotifyGraphChanged();
    return NewComment;
}

UDialogGraphNode* UDialogGraphSchema::AddDialogNode(UDialogTree* DialogTree, UEdGraph* Graph, FDialogGraphNodeData Data, const FVector2D& Location, const FText& Text, FGuid NodeGuid, bool bIsNewNode) const
{

    UDialogGraphNode* NewNode = NewObject<UDialogGraphNode>(Graph);    
    
    SetupNodeBaseState(NewNode, Data, Location, NodeGuid, bIsNewNode);

    if (DialogTree)
    {
        if (DialogTree->Sequences.Contains(NewNode->NodeGuid))
        {
            FNpcDialogSequence& Sequence = *DialogTree->Sequences.Find(NewNode->NodeGuid);
            NewNode->LevelSequenceToPlay = Sequence.LevelSequence;
            NewNode->StartPriority = Sequence.SelectionPriority;
			NewNode->Subtitles = Sequence.Subtitles;
            NewNode->bEntryPoint = Sequence.bConsideredForDialogStart;            
        }
    }

    Graph->AddNode(NewNode, true, false);

    return NewNode;
}

UPlayerChoiceNode* UDialogGraphSchema::AddPlayerChoiceNode(UDialogTree* DialogTree, UEdGraph* Graph, FDialogGraphNodeData Data, const FVector2D& Location, const FText& Text, FGuid NodeGuid, bool bIsNewNode) const
{
    UPlayerChoiceNode* NewNode = NewObject<UPlayerChoiceNode>(Graph);

    SetupNodeBaseState(NewNode, Data, Location, NodeGuid, bIsNewNode);

    if (DialogTree)
    {
        if (DialogTree->DialogChoices.Contains(NewNode->NodeGuid))
        {
            FDialogChoice& Choice = *DialogTree->DialogChoices.Find(NewNode->NodeGuid);
            NewNode->ResponseText = Choice.DisplayText;
        }
    }
    Graph->AddNode(NewNode, true, false);
    return NewNode;
}

void UDialogGraphSchema::SetupNodeBaseState(UDialogEdGraphNodeBase* NewNode, FDialogGraphNodeData Data, const FVector2D& Location, FGuid NodeGuid, bool bIsNewNode) const
{
    if (bIsNewNode)
    {
        Data.NodeTitle = NewNode->GetNodeTitle(ENodeTitleType::FullTitle);
        Data.NodeID = NodeGuid;
        Data.NodePosition.X = Location.X;
        Data.NodePosition.Y = Location.Y;
    }

    NewNode->NodeTitle = Data.NodeTitle;
    NewNode->NodeGuid = Data.NodeID;
    NewNode->AllocateDefaultPins();
    NewNode->NodePosX = Data.NodePosition.X;
    NewNode->NodePosY = Data.NodePosition.Y;
    NewNode->bShowAttributesOnNode = Data.bShowAttributesOnNode;
}


const FPinConnectionResponse UDialogGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
    // Must be opposite directions
    if (A->Direction == B->Direction)
        return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Same direction"));

    
    // can't connect to itself
    if (A->GetOwningNode() == B->GetOwningNode())
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Node can't connect to itself."));
    }

    const UEdGraphPin* OutputPin = A->Direction == EEdGraphPinDirection::EGPD_Output ? A : B;
    const UEdGraphPin*InputPin = OutputPin == A ? B : A;

    if (OutputPin->GetOwningNode()->Implements<URpgDialogNodeInterface>())
    {
        IRpgDialogNodeInterface* AsNodeInterface = Cast<IRpgDialogNodeInterface>(OutputPin->GetOwningNode());
        if (AsNodeInterface)
        {
            FPinConnectionResponse response = AsNodeInterface->CanLinkTo(OutputPin, InputPin);
            
            // Ensure Break responses target the appropriate pin
            if (response.Response == CONNECT_RESPONSE_BREAK_OTHERS_A)
            {
                return OutputPin == A ? FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, response.Message) : FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_B, response.Message);
            }

            if (response.Response == CONNECT_RESPONSE_BREAK_OTHERS_B)
            {
                return OutputPin == B ? FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, response.Message) : FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_B, response.Message);
            }

            return response;

        }
    }

    return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Output doesn't support Dialog interface!"));

}

bool UDialogGraphSchema::ArePinsCompatible(const UEdGraphPin* PinA, const UEdGraphPin* PinB, const UClass* CallingContext, bool bIgnoreArray) const
{
    const UEdGraphPin* InputPin = PinA->Direction == EEdGraphPinDirection::EGPD_Input ? PinA : PinB;
    const UEdGraphPin* OutputPin = InputPin == PinA ? PinB : PinA;

    // a node can never connect to itself
    if (OutputPin->GetOwningNode() == InputPin->GetOwningNode()) return false;
    // same direction is never compatible
    if (PinA->Direction == PinB->Direction) return false;

    // all dialog node compatibility
    if (InputPin->GetOwningNode()->IsA<UDialogGraphNode>())
    {
        if ((InputPin->PinName == UDialogGraphNode::InputPin_InDialog_Name && OutputPin->PinName == UDialogGraphNode::OutputPin_OutDialog_Name)
            || (InputPin->PinName == UDialogGraphNode::InputPin_InPlayerChoice_Name && OutputPin->PinName == UPlayerChoiceNode::OutputPin_OutDialog_Name))
        {
            return true;
        }
        return false;
    }
        
    if (InputPin->PinName == UPlayerChoiceNode::InputPin_InDialog_Name && OutputPin->PinName == UDialogGraphNode::OutputPin_OutPlayerChoice_Name)
    {
        return true;
    }

    return false;
}

FConnectionDrawingPolicy* UDialogGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
    return new FDialogGraphConnectionDrawPolicy(
        InBackLayerID,
        InFrontLayerID,
        InZoomFactor,
        InClippingRect,
        InDrawElements,
        InGraphObj
    );
}


