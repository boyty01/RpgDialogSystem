// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ActorComponent/NpcDialogComponent.h"
#include <Sound/SoundBase.h>
#include "DialogManagerSubsystem.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogDialogManagerSubsystem, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewSequence, const FNpcDialogSequence&, Sequence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSequenceFinished, const FNpcDialogSequence&, Sequence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSubtitleMarkerHit, const int, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSubtitleDialogPlay, USoundBase*, SoundFile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogFailedToStart, const FString&, Reason);
/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEM_API UDialogManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:

	// Notify that a dialog has failed to start for any reason. Should by called by the Actor who owns the Tree that failed to start.
	UFUNCTION()
	void DialogFailedToStart(AActor* InvokingActor, const FString& Reason);


	//Delegate for when a dialog fails to start for any reason.
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnDialogFailedToStart OnDialogFailedToStart;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Delegates")
	FOnSubtitleMarkerHit OnSubtitleMarkerHit;

	//Delegate for when an NPC has started playing an audio file that supports the subtitle system.
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Delegates")
	FOnSubtitleDialogPlay OnSubtitleAudioPlay;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Delegates")
	FOnNewSequence OnNewSequenceDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Delegates")
	FOnSequenceFinished OnSequenceFinishedDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Delegates")
	FOnDialogEnded OnDialogEndedDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Delegates")
	FOnDialogStarted OnDialogStartedDelegate;

	UFUNCTION(BlueprintCallable, Category ="Subtitles")
	void NotifySubtitleFilePlaying(USoundBase* SoundFile)
	{
		OnSubtitleAudioPlay.Broadcast(SoundFile);
	}

	UFUNCTION(BlueprintCallable, Category ="Dialog Manager")
	TArray<FDialogChoice> GetCurrentSequenceQualifiedChoices();

	UFUNCTION(BlueprintCallable, Category="Dialog Manager")
	void RegisterNewInteraction(AActor* DialogNPC);

	UFUNCTION(BlueprintCallable, Category="Dialog Manager")
	void NewSequence(struct FNpcDialogSequence Sequence);

	UFUNCTION(BlueprintCallable, Category="Dialog Manager")
	void SequenceFinished(struct FNpcDialogSequence sequence);

	//Skips to the end of the currently playing sequence, if one is playing.
	UFUNCTION(BLueprintCallable, Category="Dialog Manager")
	void SkipSequence();

	/*Dispatch the next sequence by the given ID.NOTE: Typically you should use SelectDialogChoice and pass in the choice struct
	 for full functionality, including script execution. */ 
	UFUNCTION(BlueprintCallable, Category="Dialog Manager")
	void MakeDialogOptionSelection(const FGuid SelectionId);

	UFUNCTION(BlueprintCallable, Category="Dialog Manager")
	void EndCurrentDialog();

	UFUNCTION(BlueprintCallable, Category ="Dialog Manager")
	bool IsDialogActive() { return bDialogIsActive; };

	/* Use this as the default option for submitting a dialog choice when you have the full struct available. Executes any script events before invoking the next sequence.
	*/
	UFUNCTION(BlueprintCallable, Category="Dialog Manager")
	void SelectDialogChoice(FDialogChoice Choice);

	UFUNCTION(BlueprintCallable, Category="Dialog Manager")
	AActor* GetActiveDialogNPC() { return ActiveDialogNPC; };

	UFUNCTION(BlueprintCallable, Category = "Dialog Manager")
	void DialogSubtitleMarkerHit(const int index) 
	{
		CurrentSubtitleIndex = index;
		OnSubtitleMarkerHit.Broadcast(index);
	}

private:

	void HandleEventData(const TArray<FDialogEventData>& Event);
	void HandleQuestEventData(APawn* TargetPawn, const FDialogEventData& Event);
	void HandleInventoryEventData(APawn* TargetPawn, const FDialogEventData& Event);
	void HandleStateEventData(const FDialogEventData& Event);
	void HandleCustomEventData(AActor* InvokingActor, const FDialogEventData& Event);	

	void GiveQuest(APawn* TargetPawn, const FName Quest);
	void RemoveQuest(APawn* TargetPawn, const FName Quest);
	void CompleteQuest(APawn* TargetPawn, const FName Quest);
	void GiveItems(APawn* TargetPawn, const FName ItemId, const int NumItems);
	void RemoveItems(APawn* TargetPawn, const FName ItemId, const int NumItems);
	void ProgressObjective(APawn* TargetPawn, const FName EventId);
	void FailQuest(APawn* TargetPawn, const FName Quest);

	bool bDialogIsActive{ false };
	bool GetPlayerPawnInterfaced(APawn*& OutPawn);
	class UNpcDialogComponent* GetActiveNPCDialogComponent();
	FNpcDialogSequence ActiveSequence;


	void ResetSubtitleIndex() { CurrentSubtitleIndex = -1; };	
	int CurrentSubtitleIndex{ -1 };

	TObjectPtr<AActor> ActiveDialogNPC;

	TMap<TSoftClassPtr<UDialogScriptEventBase>, AActor*> QueuedScriptEvents;
	TArray<UDialogScriptEventBase*> ActiveScriptEvents;

	void SpawnScriptEvent(AActor* InvokingActor, TSoftClassPtr<UDialogScriptEventBase> ScriptClass);

	void OnScriptEnded(UDialogScriptEventBase* FinishedScript);
};
