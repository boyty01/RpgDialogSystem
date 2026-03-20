// Fill out your copyright notice in the Description page of Project Settings.


#include "RpgDialogFunctionLibrary.h"

bool URpgDialogFunctionLibrary::TryConvertNameToGameplayTag(FName Name, FGameplayTag& OutTag)
{
	if (Name.IsNone())
	{
		OutTag = FGameplayTag::EmptyTag;
		return false;
	}

	OutTag = UGameplayTagsManager::Get().RequestGameplayTag(Name, false);
	return OutTag.IsValid();
}
