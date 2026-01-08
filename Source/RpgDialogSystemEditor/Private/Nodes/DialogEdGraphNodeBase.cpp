// Copyright DMTesseracT LTd. All rights reserved.


#include "Nodes/DialogEdGraphNodeBase.h"
#include "Graph/DialogGraph.h"
#include "Schema/DialogGraphSchema.h"

void UDialogEdGraphNodeBase::PostPasteNode()
{
    UE_LOG(LogTemp, Warning, TEXT("PostPasteNode called on node with old GUID: %s"), *NodeGuid.ToString());
    Super::PostPasteNode();

    if (const UDialogGraphSchema* Schema = Cast<UDialogGraphSchema>(GetGraph()->GetSchema()))
    {

        CreateNewGuid();

        TArray<UDialogEdGraphNodeBase*> NewChildNodes = ChildNodes;
        ChildNodes.Empty();

        for (UDialogEdGraphNodeBase* OriginalSub : NewChildNodes)
        {
            if (OriginalSub)
            {
                UDialogEdGraphNodeBase* NewSub = nullptr;

                // Reproduce Conditional Subnodes
                if (OriginalSub->IsA<UDialogConditionsNode>())
                {
					UDialogConditionsNode* OriginalAsConditions = Cast<UDialogConditionsNode>(OriginalSub);
                    UDialogConditionsNode* AsConditions = nullptr;
                    if (ULocalStateConditionNode* OriginalAsState = Cast<ULocalStateConditionNode>(OriginalSub))
                    {
						ULocalStateConditionNode* AsState = nullptr;
                        NewSub = AsConditions = AsState = Schema->AddConditionSubNode<ULocalStateConditionNode>(this, GetGraph());
                    }

                    if (UQuestConditionNode* OriginalAsQuest = Cast<UQuestConditionNode>(OriginalSub))
                    {
						UQuestConditionNode* AsQuest = nullptr;
                        NewSub = AsConditions = AsQuest = Schema->AddConditionSubNode<UQuestConditionNode>(this, GetGraph());
                    }

                    if (UInventoryConditionNode* OriginalAsInventory = Cast<UInventoryConditionNode>(OriginalSub))
                    {
						UInventoryConditionNode* AsInventory = nullptr;
                        NewSub = AsConditions = AsInventory = Schema->AddConditionSubNode<UInventoryConditionNode>(this, GetGraph());
                    }

                    AsConditions->Id = OriginalAsConditions->Id;
					AsConditions->bNegate = OriginalAsConditions->bNegate;
					AsConditions->RequirementType = OriginalAsConditions->RequirementType;
					AsConditions->NumberRequired = OriginalAsConditions->NumberRequired;
                    AsConditions->UpdateNodeTitle();
                }

                // Reproduce Event Subnodes 
                if (OriginalSub->IsA<UDialogEventNodeBase>())
                {
					UDialogEventNodeBase* OriginalAsEvent = Cast<UDialogEventNodeBase>(OriginalSub);
					UDialogEventNodeBase* AsEvent = nullptr;
                    
                    if (UDialogStateEventNode* OriginalAsStateEvent = Cast<UDialogStateEventNode>(OriginalSub))
                    {
						UDialogStateEventNode* AsStateEvent = nullptr;
                        NewSub = AsEvent = AsStateEvent = Schema->AddEventSubNode<UDialogStateEventNode>(this, GetGraph());
                        
					}

                    if (UDialogQuestEventNode* OriginalAsQuestEvent = Cast<UDialogQuestEventNode>(OriginalSub))
                    {
						UDialogQuestEventNode* AsQuestEvent = nullptr;
                        NewSub = AsEvent = AsQuestEvent = Schema->AddEventSubNode<UDialogQuestEventNode>(this, GetGraph());
                    }
                    
                    if (UDialogInventoryEventNode* OriginalAsInventoryEvent = Cast<UDialogInventoryEventNode>(OriginalSub))
                    {
						UDialogInventoryEventNode* AsInventoryEvent = nullptr;
                        NewSub = AsEvent = AsInventoryEvent = Schema->AddEventSubNode<UDialogInventoryEventNode>(this, GetGraph());
					}

                    if(UDialogCustomEventNode* OriginalAsCustomEvent = Cast<UDialogCustomEventNode>(OriginalSub))
                    {
						UDialogCustomEventNode* AsCustomEvent = nullptr;
                        NewSub = AsEvent = AsCustomEvent = Schema->AddEventSubNode<UDialogCustomEventNode>(this, GetGraph());
                        AsCustomEvent->CustomEventScriptClass = OriginalAsCustomEvent->CustomEventScriptClass;
					}

                    AsEvent->Id = OriginalAsEvent->Id;
					AsEvent->Value = OriginalAsEvent->Value;
					AsEvent->ActionType = OriginalAsEvent->ActionType;
					AsEvent->bCanEditValue = OriginalAsEvent->bCanEditValue;
					AsEvent->TargetSystem = OriginalAsEvent->TargetSystem;

					GetGraph()->NotifyNodeChanged(AsEvent);
                    AsEvent->UpdateNodeTitle();
                }
            }
        }
    }

    // Reconstruct if needed
    ReconstructNode();
    GetGraph()->NotifyGraphChanged();
}

void UDialogEdGraphNodeBase::DebugLogPostPaste(int32 Depth) const
{
    const FString Indent = FString::ChrN(Depth * 2, TEXT(' '));

    UE_LOG(LogTemp, Warning, TEXT("%sNode: %s | Pointer: %p | GUID: %s"),
        *Indent,
        *GetName(),
        this,
        *NodeGuid.ToString());

    for (const UDialogEdGraphNodeBase* Sub : ChildNodes)
    {
        if (Sub)
        {
            Sub->DebugLogPostPaste(Depth + 1);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("%s  SubNode is nullptr!"), *Indent);
        }
    }
}

FVector2D UDialogEdGraphNodeBase::GetNodeSize()
{
    FVector2D Size(300.f,300.f);


    return Size;
}

void UDialogEdGraphNodeBase::OnSubNodeDeleted(UDialogEdGraphNodeBase* SubNode)
{
	UE_LOG(LogTemp, Warning, TEXT("OnSubNodeDeleted called for SubNode with GUID: %s"), *SubNode->NodeGuid.ToString());
    Modify();
    if (!ChildNodes.IsEmpty())
    {
        if (UDialogGraph* DialogGraph = Cast<UDialogGraph>(GetGraph()))
        {
            DialogGraph->DeleteSubNode(SubNode);
        }
        ChildNodes.Remove(SubNode);
        UEdGraph* Graph = GetGraph();
        if (Graph)
        {
            Graph->NotifyNodeChanged(this);
        }
    }
}

void UDialogEdGraphNodeBase::OnNodeMoved()
{
	UEdGraph* Graph = GetGraph();
	if (Graph)
	{
		Graph->NotifyNodeChanged(this);
	}
}

void UDialogEdGraphNodeBase::DestroyNode()
{
    if (UDialogGraph* DialogGraph = Cast<UDialogGraph>(GetGraph()))
    {
        for (UEdGraphNode* Child : ChildNodes)
        {
            if (Child)
            {
                Child->Modify();
                DialogGraph->DeleteSubNode(Child);               
                Child->DestroyNode();
            }
        }        
    }

    ChildNodes.Empty();
    OnNodeDeletedDelegate.Broadcast(this);
    Super::DestroyNode();
};
