// Copyright DMTesseracT LTd. All rights reserved.


#include "Graph/DialogGraph.h"
#include "Interface/RpgDialogNodeInterface.h"
#include "DialogTreeEditorToolkit.h"
#include "GraphEditAction.h"
#include "Schema/DialogGraphSchema.h"

const UEdGraphSchema* UDialogGraph::GetSchema() const
{
	return GetDefault<UDialogGraphSchema>();
}

void UDialogGraph::DeleteSubNode(UEdGraphNode* SubNode)
{
	if (EditorToolkit)
	{
		RemoveNode(SubNode);
		OnNodeDeletedDelegate.Broadcast(SubNode->NodeGuid);
	}
}

void UDialogGraph::OnNodeDeleted(UEdGraphNode* Node)
{
	FEdGraphEditAction Action = FEdGraphEditAction(); 
	Action.Graph = this;
	Action.bUserInvoked = true;
	Action.Nodes.Add(Node);
	Action.Action = EEdGraphActionType::GRAPHACTION_RemoveNode;
	OnNodeDeletedDelegate.Broadcast(Node->NodeGuid);
	Super::NotifyGraphChanged(Action);
}

void UDialogGraph::NotifyGraphChanged(const FEdGraphEditAction& Action)
{
	Super::NotifyGraphChanged(Action);

	Modify();
}

void UDialogGraph::OnPinDragStarted(UEdGraphPin* SourcePin)
{
	DraggingPin = SourcePin;
}

