#pragma once 

#include "SGraphNodeComment.h"

class DialogCommentNodeStyle : public SGraphNodeComment
{

public:

	SLATE_BEGIN_ARGS(DialogCommentNodeStyle) {}
	SLATE_END_ARGS()

	void OnCommentTextCommitted(const FText& NewComment, ETextCommit::Type CommitInfo) override;
};
