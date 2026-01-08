// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/DialogGraphNode.h"
#include "EdGraph/EdGraphSchema.h"
#include "Interface/RpgDialogNodeInterface.h"
#include "DialogEventNode_Base.h"
#include "DialogEventNode_AddQuest.generated.h"



/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogEventNode_AddQuest : public UDialogEventNode_Base
{
	GENERATED_BODY()

public:
	virtual bool CanUserDeleteNode() const override { return true; };

	UDialogEventNode_AddQuest();


	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override 
	{
		return FText::FromString("Add Quest");
	}

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);

		if (UEdGraph* Graph = GetGraph())
		{
			Graph->NotifyGraphChanged();
		}
	}
#endif
};
