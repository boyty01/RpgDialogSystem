// Copyright DMTesseracT LTd. All rights reserved.

#pragma once


#include "CoreMinimal.h"
#include "Nodes/DialogEventNodeBase.h"
#include "DialogReputationEventNode.generated.h"


UENUM()
enum class EDialogReputationEventAction : uint8
{
	MODIFY_REPUTATION UMETA(DisplayName = "Grant Reputation"),
};
/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogReputationEventNode : public UDialogEventNodeBase
{
	GENERATED_BODY()
	
public:
	UDialogReputationEventNode();

	// the Reputation action to make.
	UPROPERTY(EditAnywhere)
	EDialogReputationEventAction ReputationAction;

	virtual void UpdateNodeTitle() override;

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void UpdateEventNodePostInit();
	virtual FSlateColor GetTitleBackgroundColor() const override;

	virtual void ValidateNodeConfiguration() override;
};
