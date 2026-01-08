// Copyright DMTesseracT LTd. All rights reserved.


#include "Script/DialogScriptEventBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"

UDialogScriptEventBase::UDialogScriptEventBase()
{
}

void UDialogScriptEventBase::NativeExecute(AActor* Instigator)
{
	if (!Instigator) FinishExecute(); // a valid instigator is required.

	WorldContextObject = Instigator->GetWorld();
	ExecuteScript(Instigator,WorldContextObject);
}

void UDialogScriptEventBase::FinishExecute()
{
	OnScriptEventFinished.ExecuteIfBound(this);
}


AActor* UDialogScriptEventBase::SpawnActor(const TSubclassOf<AActor> Actor, const FTransform Transform)
{
	if (!WorldContextObject) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	return WorldContextObject->SpawnActor<AActor>(Actor, Transform, Params);
}

APawn* UDialogScriptEventBase::GetPlayerPawn(const int ControllerIndex)
{

	APlayerController* PC = GetPlayerController(ControllerIndex);
	
	return PC ? PC->GetPawn() : nullptr;	
}
