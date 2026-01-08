#include "Slate/DialogCommentNodeStyle.h"
#include "Nodes/DialogCommentNode.h"

void DialogCommentNodeStyle::OnCommentTextCommitted(const FText& NewComment, ETextCommit::Type CommitInfo)
{
    if (UDialogCommentNode* CommentNode = Cast<UDialogCommentNode>(GraphNode))
    {
        const FScopedTransaction Transaction(NSLOCTEXT("DialogEditor", "EditComment", "Edit Comment"));
        CommentNode->Modify();
        CommentNode->NodeComment = NewComment.ToString();

        if (UEdGraph* Graph = CommentNode->GetGraph())
        {
            Graph->NotifyGraphChanged();
        }
    }
}
