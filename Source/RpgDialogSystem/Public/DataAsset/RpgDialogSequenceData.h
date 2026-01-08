// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DialogTypes.h"
#include "RpgDialogSequenceData.generated.h"

/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEM_API URpgDialogSequenceData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sequences")
	TArray<FNpcDialogSequence> DialogSequences;


	// Internal track of unique dialog conditions for this instance.
	UPROPERTY(SaveGame, BlueprintReadWrite, EditAnywhere, Category = "Dialog States")
	TMap<FString, bool> DialogStates;
};
