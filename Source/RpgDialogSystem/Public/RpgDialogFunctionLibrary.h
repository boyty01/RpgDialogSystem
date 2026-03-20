// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTags.h"
#include "RpgDialogFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEM_API URpgDialogFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Inventory|Tags")
	static bool TryConvertNameToGameplayTag(
		FName Name,
		FGameplayTag& OutTag
	);
};
