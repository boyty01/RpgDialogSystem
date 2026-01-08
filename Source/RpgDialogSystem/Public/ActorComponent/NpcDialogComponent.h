// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogTypes.h"
#include "DataAsset/DialogTree.h"
#include "NpcDialogComponent.generated.h"

class ULevelSequence;
class UAnimationAsset;
class UDialogScriptEventBase;


DECLARE_LOG_CATEGORY_EXTERN(LogDialogSystem, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGDIALOGSYSTEM_API UNpcDialogComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNpcDialogComponent();


	/*Starts dialog event. If a sequence is set, it should engage the dialog event. If not, it'll fall back to
	generic dialog if it's set. Otherwise it will do nothing.
	*/
	UFUNCTION(BlueprintCallable, Category="Dialog")
	void BeginInteract();

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	bool CanStartADialog();

	UFUNCTION(BlueprintCallable, Category="Dialog")
	TArray<FDialogChoice> FilterQualifiedChoices() { return GetValidDialogChoices(CurrentDialogSequence); };

	UFUNCTION(BlueprintCallable, Category="Dialog")
	bool CheckDialogCondition(const FDialogCondition& Condition);

	UFUNCTION()
	void OnSequenceFinished();

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void PlaySequenceByGuid(FGuid Id);

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void DEPRECATED_PlaySequenceAtIndex(const int Index);

	UFUNCTION(BlueprintCallable, Category ="Dialog")
	void AddDialogState(const FName Key, const bool Value);
	

	//immediately stop a sequence that's currently playing, optionally calling the SequenceFinished as if it finished naturally. if false, no notify is sent that the sequence ended.
	UFUNCTION(BlueprintCallable, Category ="Dialog")
	void StopCurrentSequence(const bool bCallSequenceFinished);

	UDialogTree* GetDialogTree() { return DialogTree; };
protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialog")
	UDialogTree* DialogTree;



	UPROPERTY()
	TObjectPtr<class ULevelSequencePlayer> ActivePlayer;


	UPROPERTY() 
	FNpcDialogSequence CurrentDialogSequence;

	// Instanced copy of Dialog State for this components owner. This is to prevent the data assets state from being overwritten.
	UPROPERTY()
	TMap<FName, bool> DialogStateModifiers;


	// Internal function that does nothing by default, Dialog begin is called when dialog is started. Useful if you need to do anything on dialog start.
	virtual void OnBeginDialog();

private:

	/** Cached marker frames: FrameNumber -> Marker Label */
	TMap<FFrameNumber, FString> SubtitleMarkers;


	/* Per-Frame checks for markers */
	void HandleSequenceUpdated(const class UMovieSceneSequencePlayer& Player,
		FFrameTime CurrentTime,
		FFrameTime PreviousTime);

	/* Binds events to the current level sequence for advancing subtitles */
	void BindSubtitleMarkerEvents();

	void UnbindSubtitleMarkerEvents();

	void OnSubtitleMarkerHit(const FString& MarkerLabel);

	int CurrentSubtitleIndex{ -1 };	

	// Internal function to end dialog. Invokes EndCurrentDialog on the subsystem. 
	void EndDialog();

	// do not call this directly. use PlaySequenceAtIndex to ensure the internal state maintains integrity.
	void PlaySequence(ULevelSequence* Sequence);

	// split a full condition string into its key/values
	bool SplitCondition(const FString ConditionString, FString& TypeOut, FString& ValueOut);

	// split a condition value into its Id/Count values.
	bool SplitValue(const FString Value, FString& IdOut, FString& NumOut);

	// get player pawn with interface qualification
	bool GetPlayerPawn(APawn*& OutPawn);

	// get owner with interface qualification
	bool GetOwnerWithInterface(AActor*& OwnerActor);

	TArray<FDialogChoice> GetValidDialogChoices(FNpcDialogSequence Node);

	void OnSequenceInitialised(FNpcDialogSequence sequence);

	class DialogManagerSubsystem* GetDialogManager();

	bool CheckInventoryCondition(const FDialogCondition& Condition);
	bool CheckQuestCondition(const FDialogCondition& Condition);
	bool CheckStateCondition(const FDialogCondition& Condition);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
