// Copyright DMTesseracT LTd. All rights reserved.

#pragma once


#include "CoreMinimal.h"
#include "Nodes/DialogEventNodeBase.h"
#include "DialogQuestEventNode.generated.h"


UENUM()
enum class EDialogQuestEventAction : uint8
{
	GIVE_QUEST,
	COMPLETE_QUEST,
	REMOVE_QUEST,
	PROGRESS_QUEST,
	FAIL_QUEST
};
/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogQuestEventNode : public UDialogEventNodeBase
{
	GENERATED_BODY()
	
public:
	UDialogQuestEventNode();

	// the quest action to make.
	UPROPERTY(EditAnywhere)
	EDialogQuestEventAction QuestAction;

	virtual void UpdateNodeTitle() override;

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void UpdateEventNodePostInit();
	virtual FSlateColor GetTitleBackgroundColor() const override;

	virtual void ValidateNodeConfiguration() override;
};
