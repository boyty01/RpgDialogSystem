// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RpgDialogNodeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URpgDialogNodeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGDIALOGSYSTEMEDITOR_API IRpgDialogNodeInterface
{
	GENERATED_BODY()

public:
	virtual FPinConnectionResponse CanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const = 0;
	
	virtual bool GetShowAttributesOnNode() const { return false; };

	virtual bool SupportsConditionsSubNode() const { return false; };

	virtual bool SupportsEventsSubNode() const { return false; };
};
