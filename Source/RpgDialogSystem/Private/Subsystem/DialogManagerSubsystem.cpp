// Copyright DMTesseracT LTd. All rights reserved.


#include "Subsystem/DialogManagerSubsystem.h"
#include "ActorComponent/NpcDialogComponent.h"
#include "Interface/DialogInterface.h"
#include "Interface/DialogInventoryInterface.h"
#include "Interface/DialogQuestInterface.h"
#include "Script/DialogScriptEventBase.h"

#if WITH_EDITOR
	#include "Subsystems/EditorActorSubsystem.h"
	#include <LevelEditor.h>
#endif

DEFINE_LOG_CATEGORY(LogDialogManagerSubsystem);

void UDialogManagerSubsystem::DialogFailedToStart(AActor* InvokingActor, const FString& Reason)
{
	if(InvokingActor->Implements<UDialogInterface>())
	{
		IDialogInterface::Execute_OnFailedToPlayDialogTree(InvokingActor, Reason);
	}

	APawn* PPawn;
	if (GetPlayerPawnInterfaced(PPawn))
	{
		IDialogInterface::Execute_OnFailedToPlayDialogTree(PPawn, Reason);
	}

	OnDialogFailedToStart.Broadcast(Reason);
}

TArray<FDialogChoice> UDialogManagerSubsystem::GetCurrentSequenceQualifiedChoices()
{
	UNpcDialogComponent* DComp = GetActiveNPCDialogComponent();
	if (DComp)
	{
		return DComp->FilterQualifiedChoices();
	}
	return TArray<FDialogChoice>();
}

void UDialogManagerSubsystem::RegisterNewInteraction(AActor* DialogNPC)
{
	ActiveDialogNPC = DialogNPC;
	bDialogIsActive = true;
	QueuedScriptEvents.Empty();
	OnDialogStartedDelegate.Broadcast();

	// Inform the player pawn and NPC that dialog interaction is beginning.
	APawn* PPawn;
	if (GetPlayerPawnInterfaced(PPawn))
	{
		IDialogInterface::Execute_BeginDialogTreeInteraction(PPawn);
	}
	
	if (ActiveDialogNPC && ActiveDialogNPC->Implements<UDialogInterface>())
	{
		IDialogInterface::Execute_BeginDialogTreeInteraction(ActiveDialogNPC);
	}
}

void UDialogManagerSubsystem::NewSequence(FNpcDialogSequence Sequence)
{
	APawn* PPawn;
	ActiveSequence = Sequence;
	if (GetPlayerPawnInterfaced(PPawn))
	{

		IDialogInterface::Execute_DialogSequenceInitalised(PPawn, Sequence);

		if(ActiveDialogNPC && ActiveDialogNPC->Implements<UDialogInterface>())
		{
			IDialogInterface::Execute_DialogSequenceInitalised(ActiveDialogNPC, Sequence);
		}

		UNpcDialogComponent* Comp = GetActiveNPCDialogComponent();

		if (!Comp)
		{
			EndCurrentDialog();
			return;
		}
		
		UDialogTree* ActiveTree = Comp->GetDialogTree();
		if (!ActiveTree)
		{
			EndCurrentDialog();
			return;
		}

		TArray<FDialogEventData> Events;
		for (auto& EventId : Sequence.TriggeredEvents)
		{
			check(ActiveTree->Events.Contains(EventId));

			Events.Add(*ActiveTree->Events.Find(EventId));
		}
		HandleEventData(Events);
		OnNewSequenceDelegate.Broadcast(Sequence);
	}
}

void UDialogManagerSubsystem::SequenceFinished(FNpcDialogSequence sequence)
{
	APawn* PPawn;
	if (GetPlayerPawnInterfaced(PPawn))
	{
		IDialogInterface::Execute_DialogSequenceEnded(PPawn, sequence);		
	}

	if (ActiveDialogNPC && ActiveDialogNPC->Implements<UDialogInterface>())
	{
		IDialogInterface::Execute_DialogSequenceEnded(ActiveDialogNPC, sequence);
	}

	OnSequenceFinishedDelegate.Broadcast(sequence);
}

void UDialogManagerSubsystem::SkipSequence()
{
	UNpcDialogComponent* Component = GetActiveNPCDialogComponent();
	if (Component)
	{
		Component->StopCurrentSequence(true);
	}
}

void UDialogManagerSubsystem::MakeDialogOptionSelection(const FGuid SelectionId)
{
	if (ActiveDialogNPC)
	{
		UNpcDialogComponent* Component = GetActiveNPCDialogComponent();
		if (Component)
		{
			Component->PlaySequenceByGuid(SelectionId);
		}

		return;
	}
	UE_LOG(LogDialogManagerSubsystem, Warning, TEXT("Failed to find dialog component on active npc"));
}

void UDialogManagerSubsystem::EndCurrentDialog()
{

	// spawn any queued script events.
	for(auto& Pair : QueuedScriptEvents)
	{
		SpawnScriptEvent(Pair.Value, Pair.Key);
	}

	QueuedScriptEvents.Empty();

	APawn* PPawn;
	if (GetPlayerPawnInterfaced(PPawn))
	{
		IDialogInterface::Execute_EndDialogTreeInteraction(PPawn);
	}

	if (ActiveDialogNPC && ActiveDialogNPC->Implements<UDialogInterface>())
	{
		IDialogInterface::Execute_EndDialogTreeInteraction(ActiveDialogNPC);	
	}

	OnDialogEndedDelegate.Broadcast();
	ActiveDialogNPC = nullptr;
	bDialogIsActive = false;
}

void UDialogManagerSubsystem::SelectDialogChoice(FDialogChoice Choice)
{
	TArray<FDialogEventData> Events;

	UNpcDialogComponent* DComp = GetActiveNPCDialogComponent();
	if (!DComp) return;

	UDialogTree* Tree = DComp->GetDialogTree();
	if (!Tree) return;
	

	for (auto& Id : Choice.TriggeredEvents)
	{
		check(Tree->Events.Contains(Id));

		Events.Add(*Tree->Events.Find(Id));
	}

	HandleEventData(Events);

	MakeDialogOptionSelection(Choice.NextSequenceId);
	return;
}

void UDialogManagerSubsystem::GiveQuest(APawn* TargetPawn, const FName Quest)
{
	if (TargetPawn->Implements<UDialogQuestInterface>())
		IDialogQuestInterface::Execute_AddQuest(TargetPawn, Quest);	
}

void UDialogManagerSubsystem::RemoveQuest(APawn* TargetPawn, const FName Quest)
{
	if (TargetPawn->Implements<UDialogQuestInterface>())
		IDialogQuestInterface::Execute_RemoveQuest(TargetPawn, Quest);
}

void UDialogManagerSubsystem::CompleteQuest(APawn* TargetPawn, const FName Quest)
{
	if (TargetPawn->Implements<UDialogQuestInterface>())
		IDialogQuestInterface::Execute_CompleteQuest(TargetPawn, Quest);
}

void UDialogManagerSubsystem::GiveItems(APawn* TargetPawn, const FName ItemId, const int NumItems)
{
	if (TargetPawn->Implements<UDialogInventoryInterface>())
		IDialogInventoryInterface::Execute_AddItem(TargetPawn, ItemId, NumItems);
}

void UDialogManagerSubsystem::RemoveItems(APawn* TargetPawn, const FName ItemId, const int NumItems)
{
	if (TargetPawn->Implements<UDialogInventoryInterface>())
		IDialogInventoryInterface::Execute_RemoveItem(TargetPawn, ItemId, NumItems);
}

void UDialogManagerSubsystem::ProgressObjective(APawn* TargetPawn, const FName EventId)
{
	if (TargetPawn->Implements<UDialogQuestInterface>())
		IDialogQuestInterface::Execute_QuestDialogSelected(TargetPawn, EventId);
}

void UDialogManagerSubsystem::FailQuest(APawn* TargetPawn, const FName Quest)
{
	if (TargetPawn->Implements<UDialogQuestInterface>())
		IDialogQuestInterface::Execute_FailQuest(TargetPawn, Quest);
}


void UDialogManagerSubsystem::HandleEventData(const TArray<FDialogEventData>& Events)
{
	APawn* Pawn;
	if (!GetPlayerPawnInterfaced(Pawn))	
	{
		UE_LOG(LogDialogManagerSubsystem, Warning, TEXT("Warning failed to get pawn when handling dialog event data."));
		return;
	}

	for (const auto& Event : Events)
	{
		switch (Event.TargetSystem)
		{
		case EDialogSystemTarget::STATE:
		{
			HandleStateEventData(Event);
			break;
		}
		case EDialogSystemTarget::QUEST:
		{
			HandleQuestEventData(Pawn, Event);
			break;
		}
		case EDialogSystemTarget::INVENTORY:
		{
			HandleInventoryEventData(Pawn, Event);
			break;
		}
		case EDialogSystemTarget::SCRIPT :
			HandleCustomEventData(ActiveDialogNPC, Event);
			break;
		
		default:
			break;
		}
	}
}

void UDialogManagerSubsystem::HandleQuestEventData(APawn* TargetPawn, const FDialogEventData& Event)
{
	if (Event.EventAction == EDialogActionType::ADD_QUEST)
	{
		GiveQuest(TargetPawn, Event.DataId);
	}
	if (Event.EventAction == EDialogActionType::REMOVE_QUEST)
	{
		RemoveQuest(TargetPawn, Event.DataId);
	}
	if (Event.EventAction == EDialogActionType::COMPLETE_QUEST)
	{
		CompleteQuest(TargetPawn, Event.DataId);	
	}
	if (Event.EventAction == EDialogActionType::PROGRESS_QUEST)
	{
		ProgressObjective(TargetPawn, Event.DataId);
	}
	if (Event.EventAction == EDialogActionType::FAIL_QUEST)
	{
		FailQuest(TargetPawn, Event.DataId);
	}
}

void UDialogManagerSubsystem::HandleInventoryEventData(APawn* TargetPawn, const FDialogEventData& Event)
{
	if (Event.EventAction == EDialogActionType::ADD_ITEM)
	{
		GiveItems(TargetPawn, Event.DataId, Event.Value);
	}
	if (Event.EventAction == EDialogActionType::REMOVE_ITEM)
	{
		RemoveItems(TargetPawn, Event.DataId, Event.Value);
	}
}

void UDialogManagerSubsystem::HandleStateEventData(const FDialogEventData& Event)
{
	UNpcDialogComponent* Comp = GetActiveNPCDialogComponent();
	if(Comp)
	{
		Comp->AddDialogState(Event.DataId, (bool) Event.Value);
	}
	
}

void UDialogManagerSubsystem::HandleCustomEventData(AActor* InvokingActor, const FDialogEventData& Event)
{
	if (Event.ScriptClass.IsNull())
	{
		return;
	}

	UE_LOG(LogDialogManagerSubsystem, Log, TEXT("Valid Script Class"));
	UClass* ScriptClass = Event.ScriptClass.LoadSynchronous();
	if (!ScriptClass)
	{
		UE_LOG(LogDialogManagerSubsystem, Log, TEXT("Failed to load script class"));
		return;
	}

	UDialogScriptEventBase* CDO = ScriptClass->GetDefaultObject<UDialogScriptEventBase>();
	if (CDO)
	{
		if (CDO->bDeferExecuteToEndOfDialogTree)
		{
			QueuedScriptEvents.Add(Event.ScriptClass, InvokingActor);
			return;
		}

		SpawnScriptEvent(InvokingActor, Event.ScriptClass);
		return;
	}
}

bool UDialogManagerSubsystem::GetPlayerPawnInterfaced(APawn*& OutPawn)
{
	OutPawn = nullptr;

	if (!GetWorld() || !GetWorld()->GetFirstPlayerController())
		return false;

	if (GetWorld()->GetFirstPlayerController()->GetPawn() &&
		GetWorld()->GetFirstPlayerController()->GetPawn()->Implements<UDialogInterface>())
	{
		OutPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	}
	return OutPawn && OutPawn->IsValidLowLevel();
}

UNpcDialogComponent* UDialogManagerSubsystem::GetActiveNPCDialogComponent()
{
	if (ActiveDialogNPC)
	{
		return ActiveDialogNPC->GetComponentByClass<UNpcDialogComponent>();
	}

	UE_LOG(LogDialogManagerSubsystem, Warning, TEXT("Missing Dialog NPC"));
	return nullptr;
}

void UDialogManagerSubsystem::SpawnScriptEvent(AActor* InvokingActor, TSoftClassPtr<UDialogScriptEventBase> ScriptClass)
{
	UDialogScriptEventBase* Script = NewObject<UDialogScriptEventBase>(this, ScriptClass.LoadSynchronous());
	if (Script)
	{

		UE_LOG(LogDialogManagerSubsystem, Log, TEXT("Spawned"));
		Script->OnScriptEventFinished.BindUObject(this, &UDialogManagerSubsystem::OnScriptEnded);
		Script->NativeExecute(InvokingActor);
		ActiveScriptEvents.Add(Script);
	}
}

void UDialogManagerSubsystem::OnScriptEnded(UDialogScriptEventBase* FinishedScript)
{
	ActiveScriptEvents.Remove(FinishedScript);
}
