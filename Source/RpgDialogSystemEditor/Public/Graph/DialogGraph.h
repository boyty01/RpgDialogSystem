// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "Nodes/DialogGraphNode.h"
#include "DialogGraph.generated.h"

class FDialogTreeEditorToolkit;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnNodeDeletedDelegate, const FGuid&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSelectedNode, UEdGraphNode*);
/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogGraph : public UEdGraph
{
	GENERATED_BODY()

public:

	FOnNodeDeletedDelegate OnNodeDeletedDelegate;
	FOnSelectedNode OnSelectedNodeDelegate;

	FDialogTreeEditorToolkit* EditorToolkit;


	const UEdGraphSchema* GetSchema() const;
	virtual bool IsNodeDeletable(UEdGraphNode* Node) const
	{
		return Node && Node->CanUserDeleteNode();
	}

	/* Get a raw ptr to the last source pin that instigated a dragdrop event. This can be null or stale. */
	UEdGraphPin* GetDraggingPin() { return DraggingPin; };

	// Broadcast that a node has been selected. Used generally for Details FNotifyHook to detect selection changes.
	void OnSelectedNode(UEdGraphNode* Node)
	{
		OnSelectedNodeDelegate.Broadcast(Node);
	}

	//handle sub nodes being deleted when a parent node is destroyed. called by the parent for each child.
	void DeleteSubNode(UEdGraphNode* SubNode);

	void OnNodeDeleted(UEdGraphNode* Node);
	virtual void NotifyGraphChanged(const FEdGraphEditAction& Action) override;

	void OnPinDragStarted(UEdGraphPin* SourcePin);

private:

	/*raw ptr to the last drag source pin. */
	UEdGraphPin* DraggingPin;
};
