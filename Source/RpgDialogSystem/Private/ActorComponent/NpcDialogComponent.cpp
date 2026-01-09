// Copyright DMTesseracT LTd. All rights reserved.


#include "ActorComponent/NpcDialogComponent.h"
#include "Interface/DialogInterface.h"
#include "Interface/DialogInventoryInterface.h"
#include "Interface/DialogQuestInterface.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "Subsystem/DialogManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogDialogSystem);


bool ShouldSkipCinematics()
{
	static const IConsoleVariable* SkipCVar =
		IConsoleManager::Get().FindConsoleVariable(TEXT("RpgDialog.AlwaysAllowSkipCinematics"));

	return SkipCVar && SkipCVar->GetInt() != 0;
}

// Sets default values for this component's properties
UNpcDialogComponent::UNpcDialogComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UNpcDialogComponent::BeginInteract()
{
	if (!GetWorld() || !GetOwner()) return;
	UDialogManagerSubsystem* Manager = GetWorld()->GetSubsystem<UDialogManagerSubsystem>();

	int CurrentIndex = -1;
	FGuid SelectedId = FGuid();
	int HighestPriority = 0;

	if (DialogTree)
	{
		for (auto& Sequence : DialogTree->Sequences)
		{
			CurrentIndex++;
			if (Sequence.Value.bConsideredForDialogStart)
			{
				bool Qualified = true;
				for (auto& Condition : Sequence.Value.QualifyingConditions)
				{
					if (!CheckDialogCondition(*DialogTree->Conditions.Find(Condition)))
					{
						Qualified = false;
						break;
					}
				}

				if (Qualified && Sequence.Value.SelectionPriority >= HighestPriority)
				{
					UE_LOG(LogDialogSystem, Verbose, TEXT("Qualified dialog sequence: %s"), *Sequence.Value.SequenceId.ToString());
					HighestPriority = Sequence.Value.SelectionPriority;
					SelectedId = Sequence.Key;
				}
			}
		}

		// if sequence is valid, play.
		if (SelectedId.IsValid())
		{
			UE_LOG(LogDialogSystem, Verbose, TEXT("Starting dialog with sequence: %s"), *SelectedId.ToString());
			
			if (!Manager) return;

			Manager->RegisterNewInteraction(GetOwner());
			PlaySequenceByGuid(SelectedId);
			return;
		}


	}

	// if invalid, we call the failure event on the owner and the player pawn.
	if (AActor* Owner = GetOwner())
	{
		Manager->DialogFailedToStart(Owner, "No qualifying dialog sequence found.");
	}
}

bool UNpcDialogComponent::CanStartADialog()
{

	int CurrentIndex = -1;
	FGuid SelectedSequenceId = FGuid();
	int HighestPriority = 0;

	if (!DialogTree) return false;

	for (auto& Sequence : DialogTree->Sequences)
	{
		CurrentIndex++;
		if (Sequence.Value.bConsideredForDialogStart)
		{
			bool Qualified = true;
			for (auto& Condition : Sequence.Value.QualifyingConditions)
			{
				if (!CheckDialogCondition(*DialogTree->Conditions.Find(Condition)))
				{
					Qualified = false;

				}
			}

			if (Qualified && Sequence.Value.SelectionPriority >= HighestPriority)
			{
				HighestPriority = Sequence.Value.SelectionPriority;
				SelectedSequenceId = Sequence.Key;
			}
		}
	}

	return SelectedSequenceId.IsValid();
}

bool UNpcDialogComponent::CheckDialogCondition(const FDialogCondition& Condition)
{
	FString CType;
	FString CValue;

	switch (Condition.TargetSystem)
	{
	case EDialogSystemTarget::INVENTORY :
		return CheckInventoryCondition(Condition);

	case EDialogSystemTarget::QUEST : 
		return CheckQuestCondition(Condition);
	
	case EDialogSystemTarget::STATE : 
		return CheckStateCondition(Condition);
	default:
		break;
	}

	return false;

}

void UNpcDialogComponent::OnSequenceFinished()
{
	bool bHasValidDialogOption = false;
	bool bHasQualifyingConditions = false;

	TArray<FDialogChoice> ValidChoices = GetValidDialogChoices(CurrentDialogSequence);
	
	// Quit if no dialog choices and no follow-up sequence
	if (!CurrentDialogSequence.AutoSelectNextSequence.IsValid() && ValidChoices.IsEmpty())
	{
		UE_LOG(LogDialogManagerSubsystem, VeryVerbose, TEXT("No dialog choices available. Ending dialog."));
		EndDialog();
		return;
	}

	if (!GetWorld()) return;
	UDialogManagerSubsystem* Manager = GetWorld()->GetSubsystem<UDialogManagerSubsystem>();
	if (Manager)
	{
		Manager->SequenceFinished(CurrentDialogSequence);
	}
	
	// if we have a valid next dialog ID and no dialog choices, then play the next dialog.
	if (CurrentDialogSequence.AutoSelectNextSequence.IsValid() && !bHasValidDialogOption)
	{
		PlaySequenceByGuid(CurrentDialogSequence.AutoSelectNextSequence);
	}

	return;
}

void UNpcDialogComponent::PlaySequenceByGuid(FGuid Id)
{

	// end if the passed Id is invalid. usually from dialog choices that end the conversation without a final sequence attached.
	if (!Id.IsValid())
	{
		EndDialog();
		return;
	}

	if (!DialogTree->Sequences.Contains(Id))
	{
		UE_LOG(LogDialogSystem, Error, TEXT("Bad Id for dialog sequence Play request. Ending Dialog! State may be broken. Given ID: %s"), *Id.ToString());
		EndDialog();
		return;
	}

	
	CurrentDialogSequence = *DialogTree->Sequences.Find(Id);
	OnSequenceInitialised(CurrentDialogSequence);
	


	PlaySequence(CurrentDialogSequence.LevelSequence.LoadSynchronous()); // There's potential for this to cause stutter. Consider changing to async.
}

void UNpcDialogComponent::StopCurrentSequence(const bool bCallSequenceFinished)
{
	if (ActivePlayer.Get() && ActivePlayer.Get()->IsPlaying() && (CurrentDialogSequence.bSkippable || ShouldSkipCinematics()))
	{		
		ActivePlayer->Stop();
		ActivePlayer->RestoreState();

		if (bCallSequenceFinished) ActivePlayer->OnFinished.Broadcast();
	}
}

// Called when the game starts
void UNpcDialogComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* OwnerActorInterfaced = nullptr;
	if (!GetOwnerWithInterface(OwnerActorInterfaced))
	{
		UE_LOG(LogDialogSystem, Error, TEXT("Error. NPC Dialog component owner: %s doesn't implement DialogInterface."), *GetOwner()->GetName());
		return;
	};

	// 
	if (DialogTree && DialogStateModifiers.IsEmpty())
	{
		DialogStateModifiers = DialogTree->DialogStates;
	}
	
}

void UNpcDialogComponent::DEPRECATED_PlaySequenceAtIndex(const int Index)
{
	// DEPRECATED 

	//UE_LOG(LogClass, Log, TEXT("Attempting to play sequence at index %d."), Index);
	//if (!DialogTree->DialogTree.IsValidIndex(Index))
	//{
	//	UE_LOG(LogClass, Warning, TEXT("Out of bounds index provided for dialog sequence play."));
	//	EndDialog();
	//	return;
	//}

	//CurrentDialogSequence = DialogTree->DialogTree[Index];
	//OnSequenceInitialised(CurrentDialogSequence);
	//PlaySequence(DialogTree->DialogTree[Index].LevelSequence);
	//return;
}

void UNpcDialogComponent::AddDialogState(const FName Key, const bool Value)
{
	UE_LOG(LogDialogSystem, Verbose, TEXT("Dialog state Modified"));
	DialogStateModifiers.Add(Key, Value);
};

void UNpcDialogComponent::OnBeginDialog()
{
	BindSubtitleMarkerEvents();
}

void UNpcDialogComponent::HandleSequenceUpdated(const UMovieSceneSequencePlayer& Player,
	FFrameTime CurrentTime,
	FFrameTime PreviousTime)
{
	// Loop through all markers
	for (const TPair<FFrameNumber, FString>& Pair : SubtitleMarkers)
	{
		FFrameNumber MarkerFrame = Pair.Key;
		UMovieScene* MovieScene = Player.GetSequence()->GetMovieScene();
		FFrameRate TickResolution = MovieScene->GetTickResolution(); 
		FFrameRate DisplayRate = MovieScene->GetDisplayRate();       

		// Convert marker frame (tick resolution) display frame
		double MarkerTimeSeconds = TickResolution.AsSeconds(MarkerFrame);
		FFrameTime MarkerDisplayFrameTime = DisplayRate.AsFrameTime(MarkerTimeSeconds);
		FFrameNumber MarkerDisplayFrame = MarkerDisplayFrameTime.FloorToFrame(); 

		// Check if the playhead crossed this marker this update
		if (PreviousTime.GetFrame() < MarkerDisplayFrame && CurrentTime.GetFrame() >= MarkerDisplayFrame)
		{
			OnSubtitleMarkerHit(Pair.Value);
		}
	}
}

void UNpcDialogComponent::BindSubtitleMarkerEvents()
{
	if (ActivePlayer)
	{
		ActivePlayer->OnSequenceUpdated().AddUObject(
			this, &UNpcDialogComponent::HandleSequenceUpdated
		);

		// Cache all markers
		ULevelSequence* Sequence = Cast<ULevelSequence>(ActivePlayer->GetSequence());
		if (Sequence)
		{
			UMovieScene* MovieScene = Sequence->GetMovieScene();
			if (MovieScene)
			{
				for (const FMovieSceneMarkedFrame& Marker : MovieScene->GetMarkedFrames())
				{
					SubtitleMarkers.Add(Marker.FrameNumber, Marker.Label);
				}
			}
		}

	}
}

void UNpcDialogComponent::UnbindSubtitleMarkerEvents()
{
	CurrentSubtitleIndex = -1;
	SubtitleMarkers.Empty();
	if (ActivePlayer)
	{
		ActivePlayer->OnSequenceUpdated().RemoveAll(this);
	}
}

void UNpcDialogComponent::OnSubtitleMarkerHit(const FString& MarkerLabel)
{
	CurrentSubtitleIndex++;

	if (MarkerLabel.ToUpper() != "SUB") // not a subtitle marker
	{
		return;
	}

	UDialogManagerSubsystem* Manager = GetWorld()->GetSubsystem<UDialogManagerSubsystem>();
	if (Manager)
	{
		Manager->DialogSubtitleMarkerHit(CurrentSubtitleIndex);
	}
	return;
}

void UNpcDialogComponent::EndDialog()
{
	CurrentSubtitleIndex = -1;
	UnbindSubtitleMarkerEvents();
	if (!GetWorld()) return;
	UDialogManagerSubsystem* Manager = GetWorld()->GetSubsystem<UDialogManagerSubsystem>();
	if (Manager)
	{
		Manager->EndCurrentDialog();
	}
	return;
}

void UNpcDialogComponent::PlaySequence(ULevelSequence* Sequence)
{
	if (Sequence)
	{
		// Play the sequence using the Level Sequence Player
		if (ActivePlayer)
		{
			ActivePlayer->OnStop.RemoveAll(this);
			UnbindSubtitleMarkerEvents();
		}
		ALevelSequenceActor* SequenceActor; 
		ActivePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), Sequence, FMovieSceneSequencePlaybackSettings(), SequenceActor);
		
		if (!ActivePlayer)
		{
			EndDialog();
			return;
		}

		BindSubtitleMarkerEvents();
		FScriptDelegate OnStopDelegate;
		OnStopDelegate.BindUFunction(this, "OnSequenceFinished");
		ActivePlayer->OnStop.AddUnique(OnStopDelegate);
	//	ActivePlayer->OnFinished.AddUnique(OnStopDelegate);
		FMovieSceneSequencePlaybackSettings Settings; Settings.FinishCompletionStateOverride = EMovieSceneCompletionModeOverride::ForceRestoreState;
		ActivePlayer->SetPlaybackSettings(Settings);
		ActivePlayer->Play();
	}
	return;
}


bool UNpcDialogComponent::SplitCondition(const FString ConditionString, FString& TypeOut, FString& ValueOut)
{
	return ConditionString.Split(":", &TypeOut, &ValueOut);
}

bool UNpcDialogComponent::SplitValue(const FString Value, FString& IdOut, FString& NumOut)
{
	return Value.Split("=", &IdOut, &NumOut);
}

bool UNpcDialogComponent::GetPlayerPawn(APawn*& OutPawn)
{
	OutPawn = nullptr;

	if(!GetWorld() || !GetWorld()->GetFirstPlayerController())
		return false;
	
	if (GetWorld()->GetFirstPlayerController()->GetPawn() && 
		GetWorld()->GetFirstPlayerController()->GetPawn()->Implements<UDialogInterface>())
	{
		OutPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	}

	return OutPawn && OutPawn->IsValidLowLevel();
}

bool UNpcDialogComponent::GetOwnerWithInterface(AActor*& OwnerActor)
{
	if (GetOwner() && GetOwner()->Implements<UDialogInterface>())
	{
		OwnerActor = GetOwner();
		return true;
	}

	OwnerActor = nullptr;
	return false;
}

TArray<FDialogChoice> UNpcDialogComponent::GetValidDialogChoices(FNpcDialogSequence Node)
{
	TArray<FDialogChoice> ValidChoices;

	for (auto& ChoiceId : Node.DialogChoices)
	{
		check(DialogTree->DialogChoices.Contains(ChoiceId));

		FDialogChoice& Choice = *DialogTree->DialogChoices.Find(ChoiceId);
		bool Qualified = true;
		for (auto& ConditionId : Choice.QualifyingConditions)
		{
			check(DialogTree->Conditions.Contains(ConditionId));

			FDialogCondition& Condition = *DialogTree->Conditions.Find(ConditionId);
			if (!CheckDialogCondition(Condition))
			{
				Qualified = false;
			}
		}
		if (Qualified)
		{
			ValidChoices.Add(Choice);
		}
	}
	return ValidChoices;
}

void UNpcDialogComponent::OnSequenceInitialised(FNpcDialogSequence sequence)
{
	if (!GetWorld()) return;

	UDialogManagerSubsystem* Manager = GetWorld()->GetSubsystem<UDialogManagerSubsystem>();
	if (Manager)
	{
		Manager->NewSequence(sequence);
	}
	return;
}

bool UNpcDialogComponent::CheckInventoryCondition(const FDialogCondition& Condition)
{
	APawn* PlayerPawn = nullptr;
	if (!GetPlayerPawn(PlayerPawn) || !PlayerPawn->Implements<UDialogInventoryInterface>())
		return false;
	bool Qualified = IDialogInventoryInterface::Execute_HasNumItem(PlayerPawn, Condition.DataId, Condition.Value);

	return Condition.bNegate ? !Qualified : Qualified;
}

bool UNpcDialogComponent::CheckQuestCondition(const FDialogCondition& Condition)
{
	APawn* PlayerPawn = nullptr;
	if (!GetPlayerPawn(PlayerPawn) || !PlayerPawn->Implements<UDialogQuestInterface>())
		return false;

	bool Qualified = false;

	switch (Condition.ConditionType)
	{
	case EDialogConditionRequirementType::HAVE:
		Qualified = IDialogQuestInterface::Execute_HasQuest(PlayerPawn, Condition.DataId);
		UE_LOG(LogDialogSystem, VeryVerbose, TEXT("HasQuest %s - Qualified? %s"), Condition.bNegate ? TEXT("(Negated)") : TEXT(""), Qualified ? TEXT("TRUE") : TEXT("FALSE"));
		break;
	case EDialogConditionRequirementType::HAS_COMPLETED:
		Qualified = IDialogQuestInterface::Execute_CompletedQuest(PlayerPawn, Condition.DataId);
		UE_LOG(LogDialogSystem, VeryVerbose, TEXT("HasCompleted %s - Qualified? %s"), Condition.bNegate ? TEXT("(Negated)") : TEXT(""), Qualified ? TEXT("TRUE") : TEXT("FALSE"));
		break;

	case EDialogConditionRequirementType::IS_COMPLETE:
		Qualified = IDialogQuestInterface::Execute_IsQuestReadyForTurnIn(PlayerPawn, Condition.DataId);
		UE_LOG(LogDialogSystem, VeryVerbose, TEXT("IsReadyForTurnIn %s - Qualified? %s"), Condition.bNegate ? TEXT("(Negated)") : TEXT(""), Qualified ? TEXT("TRUE") : TEXT("FALSE"));
		break;

	case EDialogConditionRequirementType::HAS_FAILED:
		Qualified = IDialogQuestInterface::Execute_HasFailedQuest(PlayerPawn, Condition.DataId);
		UE_LOG(LogDialogSystem, VeryVerbose, TEXT("HasFailed %s - Qualified? %s"), Condition.bNegate ? TEXT("(Negated)") : TEXT(""), Qualified ? TEXT("TRUE") : TEXT("FALSE"));
		break;
	default:
		break;
	}

	
	return Condition.bNegate ? !Qualified : Qualified;
	
}

bool UNpcDialogComponent::CheckStateCondition(const FDialogCondition& Condition)
{
	if (!DialogTree || !DialogStateModifiers.Contains(Condition.DataId)) return false;

	bool State = *DialogStateModifiers.Find(Condition.DataId);

	return Condition.bNegate ? !State : State;
}

// Called every frame
void UNpcDialogComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

