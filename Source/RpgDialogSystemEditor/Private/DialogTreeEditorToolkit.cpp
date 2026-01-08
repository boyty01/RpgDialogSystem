#include "DialogTreeEditorToolkit.h"
#include "DataAsset/DialogTree.h"
#include "Widgets/Docking/SDockTab.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Framework/Docking/TabManager.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "EditorStyleSet.h"
#include "BlueprintEditor.h"
#include "Graph/DialogGraph.h"
#include "DialogTypes.h"
#include "EdGraph/EdGraph.h"
#include "Nodes/DialogGraphNode.h"
#include "Nodes/PlayerChoiceNode.h"
#include "GraphEditAction.h"
#include "Nodes/DialogEventNodeBase.h"
#include "Nodes/DialogConditionsNode.h"
#include "Editor/Transactor.h"
#include "Editor.h"
#include "EventNodeCustomDetail.h"
#include "Schema/DialogGraphSchema.h"
#include <EdGraphUtilities.h>

#define LOCTEXT_NAMESPACE "FDialogTreeEditorToolkit"

const FName GraphTabID = (TEXT("DialogTreeGraphTab"));




void FDialogTreeEditorToolkit::Initialize(UDialogTree* InDialogTree, const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
    bGraphInitialised = false;
    check(InDialogTree);
    DialogTree = InDialogTree;

    if (GEditor)
    {
        GEditor->RegisterForUndo(this);
    }

    EditorGraph = Cast<UDialogGraph>(DialogTree->EditorGraph);

    if (!EditorGraph)
    {
       EditorGraph = NewObject<UDialogGraph>(
            DialogTree, UDialogGraph::StaticClass(), NAME_None, RF_Transactional | RF_Public);
    }
       EditorGraph->Schema = UDialogGraphSchema::StaticClass(); 
       EditorGraph->EditorToolkit = this;     
       GraphChangedDelegateHandle = EditorGraph->AddOnGraphChangedHandler(FOnGraphChanged::FDelegate::CreateSP(SharedThis(this), &FDialogTreeEditorToolkit::OnGraphChanged));
       EditorGraph->OnNodeDeletedDelegate.AddSP(SharedThis(this), &FDialogTreeEditorToolkit::OnNodeDeletedFromGraph);
       EditorGraph->OnSelectedNodeDelegate.AddSP(SharedThis(this), &FDialogTreeEditorToolkit::OnNodeSelected);   
       EditorGraph->bEditable = true;
       GraphSchema = Cast<UDialogGraphSchema>(EditorGraph->GetSchema());
       DialogTree->EditorGraph = EditorGraph;
   
  
    // Create the property editor
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    HookInstance = MakeShared<FDialogGraphDetailsNotifyHook>(EditorGraph, DialogTree);
    HookInstance->SelectedDetailsObject = DialogTree;

    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bHideSelectionTip = true;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = true;
    DetailsViewArgs.bShowOptions = true;   
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    DetailsViewArgs.NotifyHook = HookInstance.Get();
    DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    DetailsView->SetObject(DialogTree);
   
	SGraphEditor::FGraphEditorEvents GraphEditorEvents;
    GraphEditorEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(SharedThis(this), &FDialogTreeEditorToolkit::OnNodeTitleCommitted);

    FGraphAppearanceInfo Appearance;
    Appearance.CornerText = FText::FromString("DIALOG EDITOR");

    GraphEditorWidget = SNew(SDialogGraphEditor)
        .GraphToEdit(EditorGraph)
        .GraphEvents(GraphEditorEvents)
        .DetailsView(DetailsView)
        .Appearance(Appearance);


    // Bind comment action
    GraphEditorWidget->OnRequestCreateComment.BindSP(SharedThis(this), &FDialogTreeEditorToolkit::HandleCreateComment);
	GraphEditorWidget->OnRequestCopyNode.BindSP(SharedThis(this), &FDialogTreeEditorToolkit::HandleCopySelectedNodes);
	GraphEditorWidget->OnRequestCutNode.BindSP(SharedThis(this), &FDialogTreeEditorToolkit::HandleCutSelectedNodes);
	GraphEditorWidget->OnRequestPasteNode.BindSP(SharedThis(this), &FDialogTreeEditorToolkit::HandlePasteNodes);
	GraphEditorWidget->OnRequestDuplicateNode.BindSP(SharedThis(this), &FDialogTreeEditorToolkit::HandleDuplicateNodes);

    // Initialize editor
    const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("DialogTreeEditorLayout_v1")
        ->AddArea(
            FTabManager::NewPrimaryArea()
            ->SetOrientation(Orient_Horizontal)
            ->Split(
                FTabManager::NewStack()
                ->AddTab(GraphTabID, ETabState::OpenedTab)
                ->SetHideTabWell(true)
            )
        );

  //  BuildNodeGraphFromAssetData();

    TSharedRef<FExtender> ToolbarExtender = MakeShareable(new FExtender);
    ToolbarExtender->AddToolBarExtension(
        "Asset", // Insert after "Asset" section, which is where Save/Browse live
        EExtensionHook::After,
        GetToolkitCommands(), 
        FToolBarExtensionDelegate::CreateSP(this, &FDialogTreeEditorToolkit::AddToolbarButtons)
    );
    AddToolbarExtender(ToolbarExtender); // Adds it to the internal list used by InitAssetEditor
    
    InitAssetEditor(Mode, InitToolkitHost, FName("DialogTreeEditorApp"), Layout, true, true, InDialogTree);

    RegenerateMenusAndToolbars();
    bGraphInitialised = true;
}

void FDialogTreeEditorToolkit::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged)
{

    if (UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(NodeBeingChanged))
    {
        const FScopedTransaction Transaction(NSLOCTEXT("DialogEditor", "RenameCommentNode", "Rename Comment Node"));

        CommentNode->Modify();
        CommentNode->NodeComment = NewText.ToString();

        if (UEdGraph* Graph = CommentNode->GetGraph())
        {
            Graph->Modify();
            Graph->NotifyGraphChanged();
        }

        // Mark the asset dirty
        if (DialogTree)
        {
            DialogTree->MarkPackageDirty();
        }
    }
};

void FDialogTreeEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
    FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

    InTabManager->RegisterTabSpawner(GraphTabID, FOnSpawnTab::CreateSP(this, &FDialogTreeEditorToolkit::SpawnGraphTab))
        .SetDisplayName(LOCTEXT("DialogTreeGraphTab", "Dialog Tree Graph"))
        .SetGroup(WorkspaceMenuCategory.ToSharedRef())
        .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FDialogTreeEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
    FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
    InTabManager->UnregisterTabSpawner(GraphTabID);
}

TSharedRef<SDockTab> FDialogTreeEditorToolkit::SpawnGraphTab(const FSpawnTabArgs& Args)
{
    check(GraphEditorWidget.IsValid());

    return SNew(SDockTab)
        .Label(LOCTEXT("DialogTreeGraphTab_Label", "Dialog Tree Graph"))
        [
            GraphEditorWidget.ToSharedRef()
        ];
}

void FDialogTreeEditorToolkit::BuildNodeGraphFromAssetData()
{
	// We just save the Graph itself into the DialogTree asset when serializing, so no need to rebuild here. 
}

void FDialogTreeEditorToolkit::AddToolbarButtons(FToolBarBuilder& ToolbarBuilder)
{
    {
        ToolbarBuilder.AddToolBarButton(
            FUIAction(FExecuteAction::CreateSP(this, &FDialogTreeEditorToolkit::OnShowAssetDetails)),
            NAME_None,
            LOCTEXT("ShowDetailsLabel", "Dialog States"),
            LOCTEXT("ShowDetailsTooltip", "Edit the Dialog States for this tree."),
            FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details")
        );
    }
}

void FDialogTreeEditorToolkit::SetDetailsViewMode(EMenuMode Mode)
{
    if (!DetailsView.IsValid() || !DialogTree)
        return;

    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    PropertyModule.UnregisterCustomClassLayout(UDialogTree::StaticClass()->GetFName());
    PropertyModule.RegisterCustomClassLayout(
        UDialogTree::StaticClass()->GetFName(),
        FOnGetDetailCustomizationInstance::CreateStatic(&FDialogTreeCustomDetail::MakeInstance, Mode)
    );

    DetailsView->ForceRefresh();
}

EEventDetailsMode FDialogTreeEditorToolkit::GetModeForEventNode(UDialogEventNodeBase* Node)
{
    if(!Node)
		return EEventDetailsMode::Everything;

    if(Node->IsA<UDialogInventoryEventNode>())
		return EEventDetailsMode::InventoryView;

    if (Node->IsA<UDialogQuestEventNode>())
        return EEventDetailsMode::QuestView;
    
    if(Node->IsA<UDialogStateEventNode>())
		return EEventDetailsMode::StatesView;

	if (Node->IsA<UDialogCustomEventNode>())
		return EEventDetailsMode::CustomScriptView;

    return EEventDetailsMode::Everything;
}

void FDialogTreeEditorToolkit::SetDialogConditionNodeDetailsCustomization(UDialogConditionsNode* Node)
{
    if (!DetailsView.IsValid() || !Node)
        return;

    EConditionDetailsMode Mode = GetModeForConditionNode(Node);

    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.UnregisterCustomClassLayout(UDialogConditionsNode::StaticClass()->GetFName());
    PropertyModule.RegisterCustomClassLayout(
        UDialogConditionsNode::StaticClass()->GetFName(),
        FOnGetDetailCustomizationInstance::CreateStatic(&FConditionNodeCustomDetail::MakeInstance, Mode)
    );
    DetailsView->ForceRefresh();
}

EConditionDetailsMode FDialogTreeEditorToolkit::GetModeForConditionNode(UDialogConditionsNode* Node)
{

    if (!Node)
        return EConditionDetailsMode::Everything;

    if (Node->IsA<UQuestConditionNode>())
    {
        return EConditionDetailsMode::QuestView;
    }

    if (Node->IsA<ULocalStateConditionNode>())
    {
        return EConditionDetailsMode::StatesView;
    }

    if (Node->IsA<UInventoryConditionNode>())
    {
        return EConditionDetailsMode::InventoryView;
    }

    return EConditionDetailsMode::Everything;
}

void FDialogTreeEditorToolkit::SetDialogEventNodeDetailsCustomization(UDialogEventNodeBase* Node)
{
    if (!DetailsView.IsValid() || !Node)
        return;

    EEventDetailsMode Mode = GetModeForEventNode(Node);

    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.UnregisterCustomClassLayout(UDialogEventNodeBase::StaticClass()->GetFName());
    PropertyModule.RegisterCustomClassLayout(
        UDialogEventNodeBase::StaticClass()->GetFName(),
        FOnGetDetailCustomizationInstance::CreateStatic(&FEventNodeCustomDetail::MakeInstance, Mode)
    );
	DetailsView->ForceRefresh();
    //DetailsView->SetObject(Node);

}

void FDialogTreeEditorToolkit::FilterNodeByClass(UEdGraphNode* Node,  TArray<UDialogGraphNode*>& DialogNodes, TArray<UPlayerChoiceNode*>& PlayerChoiceNodes, TArray<UDialogEventNodeBase*>& EventNodes, TArray<UDialogConditionsNode*>& ConditionNodes)
{
    if (Node->IsA<UDialogGraphNode>())
    {
        DialogNodes.Add(Cast<UDialogGraphNode>(Node));
        return;
    }

    if (Node->IsA<UPlayerChoiceNode>())
    {
        PlayerChoiceNodes.Add(Cast<UPlayerChoiceNode>(Node));
        return;
    }

    if (Node->IsA<UDialogConditionsNode>())
    {
        ConditionNodes.Add(Cast<UDialogConditionsNode>(Node));
        return;
    }

    if (Node->IsA<UDialogEventNodeBase>())
    {
        EventNodes.Add(Cast<UDialogEventNodeBase>(Node));
        return;
    }

}

void FDialogTreeEditorToolkit::SerializeDialogNodes(TArray<UDialogGraphNode*> DialogNodes)
{
    for (UDialogGraphNode* Node : DialogNodes)
    {
       FNpcDialogSequence Sequence;
       Sequence.SequenceId = Node->NodeGuid;
       Sequence.bConsideredForDialogStart = Node->bEntryPoint;
	   Sequence.Subtitles = Node->Subtitles;
       Sequence.SelectionPriority = Node->StartPriority;
       Sequence.LevelSequence = Node->LevelSequenceToPlay;
	   Sequence.bSkippable = Node->bSkippable;

       TArray<FGuid> ConditionIds = ParseConditionSubNodesOnNode(Node, UDialogGraphNode::InputPin_SelectionCondition_Name);
       Sequence.QualifyingConditions = ConditionIds;

       TArray<FGuid> EventIds = ParseEventSubNodesOnNode(Node, UDialogGraphNode::InputPin_Event_Name);
       Sequence.TriggeredEvents = EventIds;

       UEdGraphPin* NextDialogPin = Node->FindPin(UDialogGraphNode::OutputPin_OutDialog_Name, EGPD_Output);

       check(NextDialogPin); 
        
       if (NextDialogPin->HasAnyConnections() && !NextDialogPin->LinkedTo.IsEmpty())
       {
           Sequence.AutoSelectNextSequence = NextDialogPin->LinkedTo[0]->GetOwningNode()->NodeGuid;
       }

       // Store all connected dialog choice IDs
       UEdGraphPin* DialogChoicePin = Node->FindPin(UDialogGraphNode::OutputPin_OutPlayerChoice_Name, EGPD_Output);
       check(DialogChoicePin);

       TArray<FGuid> DialogChoiceIds;

       if (DialogChoicePin->HasAnyConnections())
       {
           for (const auto& LinkedNode : DialogChoicePin->LinkedTo)
           {
               DialogChoiceIds.Add(LinkedNode->GetOwningNode()->NodeGuid);
           }
       }
       Sequence.DialogChoices = DialogChoiceIds;


       DialogTree->Sequences.Emplace(Sequence.SequenceId, Sequence);
    }
}

void FDialogTreeEditorToolkit::SerializePlayerChoiceNodes(TArray<UPlayerChoiceNode*>& PlayerChoiceNodes)
{
    // Gather base choice data & next dialog ID
    for (auto& ChoiceNode : PlayerChoiceNodes)
    {
        FDialogChoice DialogChoice;
        DialogChoice.DialogChoiceId = ChoiceNode->NodeGuid;
        DialogChoice.DisplayText = ChoiceNode->ResponseText;
        UEdGraphPin* OutPin_NextDialog = ChoiceNode->FindPin(UPlayerChoiceNode::OutputPin_OutDialog_Name, EGPD_Output);

        // Next Dialog 
        if (OutPin_NextDialog && OutPin_NextDialog->LinkedTo.IsValidIndex(0))
        {
            UEdGraphPin* LinkedPin = OutPin_NextDialog->LinkedTo[0];
            UDialogGraphNode* LinkedNode = Cast<UDialogGraphNode>(LinkedPin->GetOwningNode());

            check(LinkedNode);
            DialogChoice.NextSequenceId = LinkedNode->NodeGuid;
        }

        TArray<FGuid> ConditionIds = ParseConditionSubNodesOnNode(ChoiceNode, UPlayerChoiceNode::InputPin_SelectionConditions_Name);
        DialogChoice.QualifyingConditions = ConditionIds;        

        TArray<FGuid> EventIds = ParseEventSubNodesOnNode(ChoiceNode, UPlayerChoiceNode::InputPin_Events_Name);
        DialogChoice.TriggeredEvents = EventIds;

        DialogTree->DialogChoices.Emplace(DialogChoice.DialogChoiceId, DialogChoice);   
    }    
}

void FDialogTreeEditorToolkit::SerializeConditionNodes(TArray<UDialogConditionsNode*>& ConditionNodes)
{
    for (auto& ConNode : ConditionNodes)
    {
        FDialogCondition Condition;
        Condition.ConditionId = ConNode->NodeGuid;
        Condition.bNegate = ConNode->bNegate;
        Condition.DataId = ConNode->Id;
        Condition.Value = ConNode->NumberRequired;
        Condition.ConditionType = ConNode->RequirementType;
        Condition.TargetSystem = ConNode->ConditionTarget;

        DialogTree->Conditions.Emplace(Condition.ConditionId, Condition);
  }
}


void FDialogTreeEditorToolkit::SerializeEventNodes(TArray<UDialogEventNodeBase*> EventNodes)
{
    for (auto& EventNode : EventNodes)
    {
        FDialogEventData Event;
        Event.DialogEventId = EventNode->NodeGuid;
        Event.EventAction = EventNode->ActionType;
        Event.DataId = EventNode->Id;
        Event.Value = EventNode->Value;
        Event.TargetSystem = EventNode->TargetSystem;
		
        if (EventNode->IsA<UDialogCustomEventNode>())
        {
            Event.ScriptClass = Cast<UDialogCustomEventNode>(EventNode)->CustomEventScriptClass;
        }

        DialogTree->Events.Emplace(Event.DialogEventId, Event);
    }
}


TArray<FGuid> FDialogTreeEditorToolkit::ParseConditionSubNodesOnNode(UEdGraphNode* Node, FName PinName)
{


    TArray<FGuid> IdArray;
    //Gather Condition ID's
    UDialogEdGraphNodeBase* DialogNode = Cast<UDialogEdGraphNodeBase>(Node);
    TArray<UDialogConditionsNode*> SubNodes = DialogNode->GetSubNodesOfType<UDialogConditionsNode>();

    for (auto& SubNode : SubNodes)
    {
        IdArray.Add(SubNode->NodeGuid);

    }
    SerializeConditionNodes(SubNodes);
    return IdArray;

}

TArray<FGuid> FDialogTreeEditorToolkit::ParseEventSubNodesOnNode(UEdGraphNode* Node, FName PinName)
{
    TArray<FGuid> IdArray;
    //Gather Event ID's
    UDialogEdGraphNodeBase* DialogNode = Cast<UDialogEdGraphNodeBase>(Node);
    TArray<UDialogEventNodeBase*> SubNodes = DialogNode->GetSubNodesOfType<UDialogEventNodeBase>();

    for (auto& SubNode : SubNodes)
    {
        IdArray.Add(SubNode->NodeGuid);
    }
    SerializeEventNodes(SubNodes);
    return IdArray;
}

void FDialogTreeEditorToolkit::OnGraphChanged(const FEdGraphEditAction& ActionType)
{
    if(bGraphInitialised)
        UpdateDialogTreeData();
}

void FDialogTreeEditorToolkit::OnNodeDeletedFromGraph(const FGuid& Id)
{    
    DialogTree->Modify();    

    // manually remove subnodes from the runtime data
    if (DialogTree->Sequences.Contains(Id))
    {
        FNpcDialogSequence Sequence = *DialogTree->Sequences.Find(Id);
        for (auto& Condition : Sequence.QualifyingConditions)
        {
            DialogTree->Conditions.Remove(Condition);
        }

        for (auto& Event : Sequence.TriggeredEvents)
        {
            DialogTree->Events.Remove(Event);
        }
    }

    // manually remove subnodes from dialog choices
    if (DialogTree->DialogChoices.Contains(Id))
    {
        FDialogChoice Choice = *DialogTree->DialogChoices.Find(Id);
        for (auto& Condition : Choice.QualifyingConditions)
        {
            DialogTree->Conditions.Remove(Condition);
        }

        for (auto& Event : Choice.TriggeredEvents)
        {
            DialogTree->Events.Remove(Event);
        }
    }

    // remove id from the maps
    DialogTree->Sequences.Remove(Id);
    DialogTree->DialogChoices.Remove(Id);
    DialogTree->Conditions.Remove(Id);
    DialogTree->Events.Remove(Id);

    UpdateDialogTreeData();
}

void FDialogTreeEditorToolkit::OnNodeSelected(UEdGraphNode* Node)
{

    HookInstance->SelectedDetailsObject = Node;

    if (Node->IsA<UDialogEventNodeBase>())
    {
        SetDialogEventNodeDetailsCustomization(Cast<UDialogEventNodeBase>(Node));
        return;
    }

    if (Node->IsA<UDialogConditionsNode>())
    {
		SetDialogConditionNodeDetailsCustomization(Cast<UDialogConditionsNode>(Node));  
        return;
    }
}



void FDialogTreeEditorToolkit::UpdateDialogTreeData()
{
    if (!DialogTree || !bGraphInitialised) return;

    EditorGraph->Modify();
    DialogTree->Modify();

    TArray<UDialogGraphNode*> DialogNodes;
    TArray<UPlayerChoiceNode*> PlayerChoiceNodes;
    TArray<UDialogEventNodeBase*> EventNodes;
    TArray<UDialogConditionsNode*> ConditionNodes;

    // Update nodes
    for (UEdGraphNode* Node : EditorGraph->Nodes)
    {        
        FilterNodeByClass(Node, DialogNodes, PlayerChoiceNodes, EventNodes, ConditionNodes);
    }

    SerializeDialogNodes(DialogNodes);
    SerializePlayerChoiceNodes(PlayerChoiceNodes);
    SerializeConditionNodes(ConditionNodes);
    SerializeEventNodes(EventNodes);

}

void FDialogTreeEditorToolkit::HandleCreateComment()
{
    if (!EditorGraph || !GraphEditorWidget.IsValid())
    {
        return;
    }

    TSharedPtr<SGraphEditor> GraphEditor = GraphEditorWidget->GraphEditor;
    if (!GraphEditor.IsValid())
    {
        return;
    }

    const FGraphPanelSelectionSet& SelectedNodes = GraphEditor->GetSelectedNodes();

    // Convert cursor to graph space
    const FVector2D GraphMousePosition = GraphEditor->GetPasteLocation();

    // No selection. simple comment
    if (SelectedNodes.Num() == 0)
    {
        GraphSchema->CreateComment(
            EditorGraph,
            GraphMousePosition,
            FVector2D(400.f, 200.f),
            FText::FromString("Comment"),
            FGuid::NewGuid(),
            true
        );
        return;
    }

    FSlateRect SelectionBounds; 
    GraphEditor->GetBoundsForSelectedNodes(SelectionBounds, 0.f);

    FVector2D MinPos(SelectionBounds.Left, SelectionBounds.Top);
    FVector2D MaxPos(SelectionBounds.Right, SelectionBounds.Bottom);

    // Padding like Blueprint
    const float Padding = 32.f;
    MinPos -= FVector2D(Padding, Padding);
    MaxPos += FVector2D(Padding, Padding);

    const FVector2D CommentSize = MaxPos - MinPos;

    GraphSchema->CreateComment(
        EditorGraph,
        MinPos,
        CommentSize,
        FText::FromString("Comment"),
        FGuid::NewGuid(),
        true
    );
}


void FDialogTreeEditorToolkit::HandleCopySelectedNodes(FGraphPanelSelectionSet Nodes)
{
	// handled directly in SDialogGraphEditor
}

void FDialogTreeEditorToolkit::HandlePasteNodes()
{
    // handled directly in SDialogGraphEditor
}

void FDialogTreeEditorToolkit::HandleCutSelectedNodes(FGraphPanelSelectionSet Nodes)
{
    // handled directly in SDialogGraphEditor
}

void FDialogTreeEditorToolkit::HandleDuplicateNodes(FGraphPanelSelectionSet Nodes)
{
    // handled directly in SDialogGraphEditor
}

void FDialogTreeEditorToolkit::OnClose()
{
    if (EditorGraph)
    {
        EditorGraph->OnNodeDeletedDelegate.RemoveAll(this);
        EditorGraph->RemoveOnGraphChangedHandler(GraphChangedDelegateHandle);

        // unregister from undo
        if (GEditor)
        {
            GEditor->UnregisterForUndo(this);
        }
    }
}

void FDialogTreeEditorToolkit::PostUndo(bool bSuccess)
{
    if (EditorGraph)
    {
        FEdGraphEditAction Action;
        Action.bUserInvoked = false;
        EditorGraph->NotifyGraphChanged(Action);
    }
}

FName FDialogTreeEditorToolkit::GetToolkitFName() const
{
    return FName("DialogTreeEditor");
}

FText FDialogTreeEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT("AppLabel", "Dialog Tree Editor");
}

FString FDialogTreeEditorToolkit::GetWorldCentricTabPrefix() const
{
    return TEXT("DialogTree");
}

FLinearColor FDialogTreeEditorToolkit::GetWorldCentricTabColorScale() const
{
    return FLinearColor(0.2f, 0.6f, 0.4f, 0.5f);
}

#undef LOCTEXT_NAMESPACE

FDialogGraphDetailsNotifyHook::FDialogGraphDetailsNotifyHook(UDialogGraph* _Graph, UDialogTree* _Tree)
{
    Tree = _Tree;
    Graph = _Graph;
}

void FDialogGraphDetailsNotifyHook::NotifyPreChange(FProperty* PropertyAboutToChange)
{
  //  if (SelectedDetailsObject) SelectedDetailsObject->Modify();
}

void FDialogGraphDetailsNotifyHook::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{

}


