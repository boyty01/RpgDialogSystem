// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogScriptEventBase.generated.h"

/**
 * Dialog script UObject that receives the target actor as the Instigator. Is executed as necessary during or after dialog events, as specified.
 * NOTE: When finishing execution of the script, FinishExecute MUST be called to ensure proper cleanup. 
 */

DECLARE_DELEGATE_OneParam(FOnDialogScriptEventFinished, UDialogScriptEventBase* /* FinishedScriptEvent  */);

UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class RPGDIALOGSYSTEM_API UDialogScriptEventBase : public UObject
{
	GENERATED_BODY()
	
public:

	UDialogScriptEventBase();

	/* If true, this script will be deferred until the current dialog tree ends. if false, it will execute immediately when it's spawned during the dialog tree execution. 
	This is useful for situations where you want to trigger a script only after the current tree ends, rather than in the middle of it. */
	UPROPERTY(EditDefaultsOnly, Category="Execution")
	bool bDeferExecuteToEndOfDialogTree;


	FOnDialogScriptEventFinished OnScriptEventFinished;

	// Should be called to initialise the script in native code. This calls ExecuteScript for blueprint instance logic.
	void NativeExecute(AActor* Instigator);

	// called by NativeExecute to run the blueprint logic for this script event.
	UFUNCTION(BlueprintImplementableEvent, Category="Script")
	void ExecuteScript(AActor* Instigator, UWorld* World);

	// get the current world.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Script")
	UWorld* GetWorldContext() { return GetWorld(); };

	// call to ensure cleanup. informs Dialog Manager Subsystem that it can stop tracking this script instance. Call this when your script is finished executing.
	UFUNCTION(BlueprintCallable, Category="Script")
	void FinishExecute();

	virtual UWorld* GetWorld() const override
	{
		if (const UObject* OuterObj = GetOuter())
		{
			return OuterObj->GetWorld();
		}
		return nullptr;
	}

protected:

	// Helper function to spawn an actor in the world.
	UFUNCTION(blueprintCallable, Category ="Helpers")
	AActor* SpawnActor(const TSubclassOf<AActor> Actor, const FTransform Transform);

	// Helper function to get the player pawn.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category ="Helpers")
	APawn* GetPlayerPawn(const int ControllerIndex);

	// Helper function to get the player controller by index.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Helpers")
	APlayerController* GetPlayerController(const int ControllerIndex)
	{
		if (UWorld* World = GetWorld())
		{
			for (FConstPlayerControllerIterator i = World->GetPlayerControllerIterator(); i; ++i)
			{

				if (i.GetIndex() == ControllerIndex)
				{
					return i->Get();
				}
			};
		}
		return nullptr;
	}

	UWorld* WorldContextObject;
private:

};
