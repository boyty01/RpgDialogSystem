// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "Nodes/DialogGraphNode.h"
#include "DialogGraphConnectionDrawPolicy.h"
#include "Nodes/PlayerChoiceNode.h"
#include "Nodes/DialogConditionsNode.h"
#include "Nodes/QuestConditionNode.h"
#include "Nodes/InventoryConditionNode.h"
#include "Nodes/LocalStateConditionNode.h"
#include "Nodes/DialogEdGraphNodeBase.h"
#include "Nodes/DialogInventoryEventNode.h"
#include "Nodes/DialogQuestEventNode.h"
#include "Nodes/DialogStateEventNode.h"
#include "Nodes/DialogConditionsNode.h"
#include "Nodes/DialogEventNodeBase.h"
#include "Nodes/DialogCustomEventNode.h"
#include "Nodes/DialogCommentNode.h"
#include "DataAsset/DialogTree.h"
#include "DialogTypes.h"
#include "EdGraphNode_Comment.h"
#include "DialogGraphSchema.generated.h"


/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()
	
public:


    virtual void GetGraphDisplayInformation(
        const UEdGraph& Graph,
        FGraphDisplayInfo& DisplayInfo
    ) const override
    {
        DisplayInfo.PlainName = FText::FromString("Dialog Tree");
        DisplayInfo.DisplayName = FText::FromString("DIALOG TREE");
        DisplayInfo.Tooltip = FText::FromString("Handles dialog flow.");
    };

	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(
		class UToolMenu* Menu,
		class UGraphNodeContextMenuContext* Context) const override;

    virtual bool ShouldAlwaysPurgeOnModification() const { return false; };

    UEdGraphNode* AddGraphNodeFromData(UEdGraph* Graph, FDialogGraphNodeData Data, UDialogTree* DialogTree) const;

    /* Add a standard engine graph comment node */
    UDialogCommentNode* CreateComment(UEdGraph* Graph, const FVector2D& Location, const FVector2D& Size, const FText& Title, FGuid NodeId, bool bIsNewNode = false) const;
	
    /* Add a dialog node (NPC dialog) to the graph */
    UDialogGraphNode* AddDialogNode(UDialogTree* DialogTree, UEdGraph* Graph, FDialogGraphNodeData Data, const FVector2D& Location, const FText& Text, FGuid NodeGuid, bool bIsNewNode = false) const;
    
    /* Add a player choice (dialog choice) node to the graph. */
    UPlayerChoiceNode* AddPlayerChoiceNode(UDialogTree* DialogTree, UEdGraph* Graph, FDialogGraphNodeData Data, const FVector2D& Location, const FText& Text, FGuid NodeGuid, bool bIsNewNode = false) const;
   
    /* Sets up the base properties for all nodes that subclass UDialogEdGraphNode */
    void SetupNodeBaseState(UDialogEdGraphNodeBase* Node, FDialogGraphNodeData Data, const FVector2D& Location, FGuid Guid, bool bIsNewNode) const;

    /* Templated create condition node. Handles making any descendant class of UDialogConditionsNode */
    template <typename T> 
    T* CreateConditionNode(UDialogTree* DialogTree, UEdGraph * Graph, FDialogGraphNodeData Data, const FVector2D& Location, const FText& Text, FGuid NodeGuid, bool bIsNewNode = false) const
    {
        static_assert(TIsDerivedFrom<T, UDialogConditionsNode>::IsDerived,
            "T must derive from UDialogConditionsNode");

        T* NewNode = NewObject<T>(Graph);
        SetupNodeBaseState(NewNode, Data, Location, NodeGuid, bIsNewNode);
        if (DialogTree)
        {
            if (DialogTree->Conditions.Contains(NewNode->NodeGuid))
            {
                FDialogCondition& Condition = *DialogTree->Conditions.Find(NewNode->NodeGuid);

                NewNode->Id = Condition.DataId;
                NewNode->bNegate = Condition.bNegate;
                NewNode->NumberRequired = Condition.Value;
                NewNode->RequirementType = Condition.ConditionType;
            }
        }
        Graph->AddNode(NewNode, true, false);
        return NewNode;
    };
   
    /* Templated create Event node. Handles making any descendant class of UDialogEventNode */
    template <typename T>
    T* CreateEventNode(UDialogTree* DialogTree, UEdGraph* Graph, FDialogGraphNodeData Data, const FVector2D& Location, const FText& Text, FGuid NodeGuid, bool bIsNewNode = false) const
    {
        static_assert(TIsDerivedFrom<T, UDialogEventNodeBase>::IsDerived,
            "T must derive from UDialogConditionsNode");

        T* NewNode = NewObject<T>(Graph);

        SetupNodeBaseState(NewNode, Data, Location, NodeGuid, bIsNewNode);
        
        if (DialogTree->Events.Contains(NewNode->NodeGuid))
        {
            FDialogEventData& Event = *DialogTree->Events.Find(NewNode->NodeGuid);
            NewNode->Id = Event.DataId;
            NewNode->Value = Event.Value;
            NewNode->ActionType = Event.EventAction;
            NewNode->UpdateEventNodePostInit();
        }

        return NewNode;
    };

    // Instantiate Subnode of condition node and embed it in target node
    template <typename T>
    T* AddConditionSubNode(UDialogEdGraphNodeBase* Node, UEdGraph* Graph) const
    {
        static_assert (TIsDerivedFrom<T, UDialogConditionsNode>::IsDerived,
            "T Must derive from UDialogConditionsNode");

        if (!Node->SupportsConditionsSubNode()) return nullptr;

        FScopedTransaction Transaction(FText::FromString("Add SubNode"));

        T* NewSubnode = NewObject<T>(Graph, T::StaticClass(), NAME_None, RF_Transactional);
        NewSubnode->NodeGuid = FGuid::NewGuid();
        Node->Modify();
        Node->AddNode(NewSubnode);
        NewSubnode->ParentNode = Node;
      //  Graph->Nodes.Add(NewSubnode);// AddNode(NewSubnode, false, false);
        Graph->NotifyGraphChanged();

        return NewSubnode;
    }

    // Instantiate SubNode version of Event node and embed it in target node
    template <typename T>
    T* AddEventSubNode(UDialogEdGraphNodeBase* Node, UEdGraph* Graph) const
    {
        static_assert (TIsDerivedFrom<T, UDialogEventNodeBase>::IsDerived,
            "T Must derive from UDialogEventNodeBase");

        if (!Node->SupportsEventsSubNode()) return nullptr;

        FScopedTransaction Transaction(FText::FromString("Add SubNode"));

        T* NewSubnode = NewObject<T>(Graph, T::StaticClass(), NAME_None, RF_Transactional);
        NewSubnode->NodeGuid = FGuid::NewGuid();
        Node->Modify();
        Node->AddNode(NewSubnode);
        NewSubnode->ParentNode = Node;
      //  Graph->AddNode(NewSubnode, false, false);
        Graph->NotifyGraphChanged();
        return NewSubnode;
    }

    UDialogInventoryEventNode* DEPRECATED_AddInventoryEventNode(UDialogTree* DialogTree, UEdGraph* Graph, FDialogGraphNodeData Data, const FVector2D& Location, const FText& Text, FGuid NodeGuid, bool bIsNewNode = false) const;
    UDialogQuestEventNode* DEPRECATED_AddQuestEventNode(UDialogTree* DialogTree, UEdGraph* Graph, FDialogGraphNodeData Data, const FVector2D& Location, const FText& Text, FGuid NodeGuid, bool bIsNewNode = false) const;
    UDialogStateEventNode* DEPRECATED_AddStateEventNode(UDialogTree* DialogTree, UEdGraph* Graph, FDialogGraphNodeData Data, const FVector2D& Location, const FText& Text, FGuid NodeGuid, bool bIsNewNode = false) const;

    virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
    virtual bool ShouldHidePinDefaultValue(UEdGraphPin* Pin) const override
    {
        return false;
    }
    virtual bool ArePinsCompatible(const UEdGraphPin* PinA, const UEdGraphPin* PinB, const UClass* CallingContext = nullptr, bool bIgnoreArray = false) const;


    virtual FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(
        int32 InBackLayerID,
        int32 InFrontLayerID,
        float InZoomFactor,
        const FSlateRect& InClippingRect,
        FSlateWindowElementList& InDrawElements,
        UEdGraph* InGraphObj
    ) const override;


};

class FDialogGraphSchemaAction_NewComment : public FEdGraphSchemaAction
{
public:
    FDialogGraphSchemaAction_NewComment()
        : FEdGraphSchemaAction(
            FText::FromString(TEXT("Comment")),
            FText::FromString(TEXT("Add Comment")),
            FText::FromString(TEXT("Adds a comment to the graph")),
            0
        ) {
    }

    virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override
    {
        FScopedTransaction Transaction(FText::FromString("Add Comment"));
        const UDialogGraphSchema* Schema = CastChecked<UDialogGraphSchema>(ParentGraph->GetSchema());
        UDialogCommentNode* NewNode = Schema->CreateComment(ParentGraph, Location, FVector2D(100.f), FText::FromString("Comment"), FGuid::NewGuid(), true);

        if (bSelectNewNode && NewNode)
        {
        }

        NewNode->GetGraph()->NotifyGraphChanged();
        return NewNode;
    }
};



/*-----------------------*/
/*  Dialog Sequence Node */
/*-----------------------*/

/* Dialog nodes represent a single dialog sequence in which all connected non-dialog nodes are serialized into. */
class FDialogGraphSchemaAction_NewDialogNode : public FEdGraphSchemaAction
{
public:
    FDialogGraphSchemaAction_NewDialogNode()
        : FEdGraphSchemaAction(
            FText::FromString("Dialog"),                 // InNodeCategory
            FText::FromString("Add Dialog Node"),        // InMenuDesc
            FText::FromString("Adds a new dialog node"), // InToolTip
            0                                            // InGrouping
        ) {
    }

    // Called when the user selects this action from the context menu
    virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override
    {
        const UDialogGraphSchema* Schema = CastChecked<UDialogGraphSchema>(ParentGraph->GetSchema());
        UDialogGraphNode* NewNode = Schema->AddDialogNode(nullptr, ParentGraph, FDialogGraphNodeData(), Location, FText::FromString("New Dialog"), FGuid::NewGuid(), true);

        if (FromPin)
        {
            for (UEdGraphPin* Pin : NewNode->Pins)
            {
                // Schema decides what can connect
                auto Test = Schema->CanCreateConnection(FromPin, Pin);

                if (Test.Response == CONNECT_RESPONSE_MAKE ||
                    Test.Response == CONNECT_RESPONSE_BREAK_OTHERS_A)
                {
                    Schema->TryCreateConnection(FromPin, Pin);
                    break;
                }
            }
        }
        NewNode->GetGraph()->NotifyGraphChanged();
        return NewNode;
    }
};



/*-----------------------*/
/*  Dialog Choice Node   */
/*-----------------------*/
/* Dialog Choice nodes bridge Dialog Sequences and are serialized into the dialog sequence derrived from the node connected to its input pin. */
class FDialogGraphSchemaAction_PlayerChoice : public FEdGraphSchemaAction
{
public:
    FDialogGraphSchemaAction_PlayerChoice()
        : FEdGraphSchemaAction(
            FText::FromString("Dialog"),
            FText::FromString("Add Player Choice Node"),
            FText::FromString("Adds a player choice node"),
            0)
    {
    }

    virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override
    {
        const UDialogGraphSchema* Schema = CastChecked<UDialogGraphSchema>(ParentGraph->GetSchema());
        UPlayerChoiceNode* NewNode = Schema->AddPlayerChoiceNode(nullptr, ParentGraph, FDialogGraphNodeData(), Location, FText::FromString("Player Choice"), FGuid::NewGuid(), true);

        if (FromPin)
        {
            for (UEdGraphPin* Pin : NewNode->Pins)
            {
                // Schema decides what can connect
                auto Test = Schema->CanCreateConnection(FromPin, Pin);

                if (Test.Response == CONNECT_RESPONSE_MAKE ||
                    Test.Response == CONNECT_RESPONSE_BREAK_OTHERS_A)
                {
                    Schema->TryCreateConnection(FromPin, Pin);
                    break;
                }
            }
        }
        if (bSelectNewNode && NewNode)
        {
        }

        NewNode->GetGraph()->NotifyGraphChanged();
        return NewNode;
    }
};


/*---------------------*/
/*  Conditional Nodes   */
/*---------------------*/
/* Conditional Nodes represent the conditions that both sequences and choices evaluate when the dialog system assesses if they can be played/shown. They can be connected to the InCondition Pins
 * on either Dialog Nodes or Player Choice and are serialized into the appropriate struct.
 */
class FDialogGraphSchemaAction_NewQuestConditionsNode : public FEdGraphSchemaAction
{
public:
    FDialogGraphSchemaAction_NewQuestConditionsNode()
        : FEdGraphSchemaAction(
            FText::FromString("Conditions"),
            FText::FromString("Quest Condition"),
            FText::FromString("Adds a quest condition node"),
            0)
    {
    }


    virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override
    {
        const UDialogGraphSchema* Schema = CastChecked<UDialogGraphSchema>(ParentGraph->GetSchema());
        UQuestConditionNode* NewNode = Schema->CreateConditionNode<UQuestConditionNode>(nullptr, ParentGraph, FDialogGraphNodeData(), Location, FText::FromString("Required Conditions"), FGuid::NewGuid(), true);

        if (bSelectNewNode && NewNode)
        {
        }

        NewNode->GetGraph()->NotifyGraphChanged();
        return NewNode;
    }
};

class FDialogGraphSchemaAction_NewInventoryConditionsNode : public FEdGraphSchemaAction
{
public:
    FDialogGraphSchemaAction_NewInventoryConditionsNode()
        : FEdGraphSchemaAction(
            FText::FromString("Conditions"),
            FText::FromString("Inventory Condition"),
            FText::FromString("Adds an inventory condition node"),
            0)
    {
    }


    virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override
    {
        const UDialogGraphSchema* Schema = CastChecked<UDialogGraphSchema>(ParentGraph->GetSchema());
        UInventoryConditionNode* NewNode = Schema->CreateConditionNode<UInventoryConditionNode>(nullptr, ParentGraph, FDialogGraphNodeData(), Location, FText::FromString("Required Conditions"), FGuid::NewGuid(), true);

        if (bSelectNewNode && NewNode)
        {
        }

        NewNode->GetGraph()->NotifyGraphChanged();
        return NewNode;
    }
};

class FDialogGraphSchemaAction_NewStateConditionsNode : public FEdGraphSchemaAction
{
public:
    FDialogGraphSchemaAction_NewStateConditionsNode()
        : FEdGraphSchemaAction(
            FText::FromString("Conditions"),
            FText::FromString("State Condition"),
            FText::FromString("Adds a State condition node"),
            0)
    {
    }


    virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override
    {
        const UDialogGraphSchema* Schema = CastChecked<UDialogGraphSchema>(ParentGraph->GetSchema());
        ULocalStateConditionNode* NewNode = Schema->CreateConditionNode<ULocalStateConditionNode>(nullptr, ParentGraph, FDialogGraphNodeData(), Location, FText::FromString("Required Conditions"), FGuid::NewGuid(), true);

        if (bSelectNewNode && NewNode)
        {
        }
        NewNode->GetGraph()->NotifyGraphChanged();
        return NewNode;
    }
};


// events 

class FDialogGraphSchemaAction_NewInventoryEventNode : public FEdGraphSchemaAction
{
public:
    FDialogGraphSchemaAction_NewInventoryEventNode()
        : FEdGraphSchemaAction(
            FText::FromString("Events"),
            FText::FromString("Inventory Event"),
            FText::FromString("Adds an Inventory event node"),
            0)
    {
    }


    virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override
    {
        const UDialogGraphSchema* Schema = CastChecked<UDialogGraphSchema>(ParentGraph->GetSchema());
        UDialogInventoryEventNode* NewNode = Schema->CreateEventNode<UDialogInventoryEventNode>(nullptr, ParentGraph, FDialogGraphNodeData(), Location, FText::FromString("Required Conditions"), FGuid::NewGuid(), true);

        if (bSelectNewNode && NewNode)
        {
        }

        NewNode->GetGraph()->NotifyGraphChanged();
        return NewNode;
    }
};


class FDialogGraphSchemaAction_NewQuestEventNode : public FEdGraphSchemaAction
{
public:
    FDialogGraphSchemaAction_NewQuestEventNode()
        : FEdGraphSchemaAction(
            FText::FromString("Events"),
            FText::FromString("Quest Event"),
            FText::FromString("Adds an Quest event node"),
            0)
    {
    }


    virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override
    {
        const UDialogGraphSchema* Schema = CastChecked<UDialogGraphSchema>(ParentGraph->GetSchema());
        UDialogQuestEventNode* NewNode = Schema->CreateEventNode<UDialogQuestEventNode>(nullptr, ParentGraph, FDialogGraphNodeData(), Location, FText::FromString("Required Conditions"), FGuid::NewGuid(), true);

        if (bSelectNewNode && NewNode)
        {
        }

        NewNode->GetGraph()->NotifyGraphChanged();
        return NewNode;
    }
};

class FDialogGraphSchemaAction_NewStateEventNode : public FEdGraphSchemaAction
{
public:
    FDialogGraphSchemaAction_NewStateEventNode()
        : FEdGraphSchemaAction(
            FText::FromString("Events"),
            FText::FromString("State Event"),
            FText::FromString("Adds an State event node"),
            0)
    {
    }


    virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override
    {
        const UDialogGraphSchema* Schema = CastChecked<UDialogGraphSchema>(ParentGraph->GetSchema());
        UDialogStateEventNode* NewNode = Schema->CreateEventNode<UDialogStateEventNode>(nullptr, ParentGraph, FDialogGraphNodeData(), Location, FText::FromString("Required Conditions"), FGuid::NewGuid(), true);

        if (bSelectNewNode && NewNode)
        {
        }
        NewNode->GetGraph()->NotifyGraphChanged();
        return NewNode;
    }
};

class FDialogGraphSchemaAction_NewCustomEventNode : public FEdGraphSchemaAction
{
public:
    FDialogGraphSchemaAction_NewCustomEventNode()
        : FEdGraphSchemaAction(
            FText::FromString("Events"),
            FText::FromString("Custom Event"),
            FText::FromString("Adds an Custom event node"),
            0)
    {
    }
    virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override
    {
        const UDialogGraphSchema* Schema = CastChecked<UDialogGraphSchema>(ParentGraph->GetSchema());
        UDialogCustomEventNode* NewNode = Schema->CreateEventNode<UDialogCustomEventNode>(nullptr, ParentGraph, FDialogGraphNodeData(), Location, FText::FromString("Required Conditions"), FGuid::NewGuid(), true);
        if (bSelectNewNode && NewNode)
        {
        }
        NewNode->GetGraph()->NotifyGraphChanged();
        return NewNode;
    }
};