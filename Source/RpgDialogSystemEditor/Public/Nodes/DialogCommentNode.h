#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "Nodes/DialogEdGraphNodeBase.h"
#include "DialogTypes.h"
#include "Interface//RpgDialogNodeInterface.h"
#include "EdGraphNode_Comment.h"
#include "DialogCommentNode.generated.h"


UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogCommentNode : public UEdGraphNode_Comment
{
	GENERATED_BODY()

public:

	UDialogCommentNode()
	{
		SetFlags(RF_Transactional);
	}

	virtual void OnRenameNode(const FString& NewName) override;

	virtual void OnUpdateCommentText(const FString& NewComment) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};
