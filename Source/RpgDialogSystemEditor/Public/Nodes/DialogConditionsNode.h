// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/DialogEdGraphNodeBase.h"
#include "DialogTypes.h"
#include "DialogConditionsNode.generated.h"

/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogConditionsNode : public UDialogEdGraphNodeBase
{
	GENERATED_BODY()
	
public:
	UDialogConditionsNode()
	{
		bIsSubNode = true;
		UpdateNodeTitle();
	}

	static const FName OutputPin_Conditions_Name;

	UPROPERTY(VisibleAnywhere)
	EDialogSystemTarget ConditionTarget;

	UPROPERTY(EditAnywhere, meta=(EditCondition="bCanEditAction"))
	EDialogConditionRequirementType RequirementType;

	UPROPERTY()
	bool bCanEditAction{ false };

	UPROPERTY()
	bool bCanEditNumValue{ false };

	//Negates the conditional requirement (i.e. MUST_HAVE becomes MUST_NOT_HAVE)
	UPROPERTY(EditAnywhere)
	bool bNegate;
	
	// ID of the condition type (i.e. Quest/Item ID)
	UPROPERTY(EditAnywhere)
	FName Id;

	//Number of the ID required. This is only available for select conditions, namely inventory "N" item checks.
	UPROPERTY(EditAnywhere, meta=(ClampMin = "1", EditCondition="bCanEditValue"))
	int NumberRequired{1};

	virtual FString GetSerializedString() const { return FString(); };

	virtual void AllocateDefaultPins() override;
	virtual FPinConnectionResponse CanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const override;
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
	virtual void UpdateNodeTitle();
};
