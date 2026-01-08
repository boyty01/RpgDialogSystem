#include "Nodes/DialogCommentNode.h"

void UDialogCommentNode::OnRenameNode(const FString& NewName)
{
	Super::OnRenameNode(NewName);
    Modify();
    NodeComment = NewName;
    // Notify the graph
    if (UEdGraph* Graph = GetGraph())
    {
        Graph->NotifyGraphChanged();
    }
}

void UDialogCommentNode::OnUpdateCommentText(const FString& NewComment)
{
}

void UDialogCommentNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName PropertyName = PropertyChangedEvent.GetPropertyName();

    if (PropertyName == GET_MEMBER_NAME_CHECKED(UEdGraphNode_Comment, NodeComment))
    {
        // Ensure the graph knows something changed
        if (UEdGraph* Graph = GetGraph())
        {
            Graph->NotifyGraphChanged();
        }

		
    }
}
