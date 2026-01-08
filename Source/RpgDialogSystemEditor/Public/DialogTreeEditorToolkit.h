
#pragma once 

#include "Toolkits/IToolkitHost.h"
#include "UI/DialogGraphEditor.h"
#include "DialogTreeCustomDetail.h"
#include "EventNodeCustomDetail.h"
#include "ConditionNodeCustomDetail.h"

class UDialogTree;
class UDialogGraphNode;
class UPlayerChoiceNode;
class UDialogEventNodeBase;
class UDialogConditionsNode;
class UDialogGraph;
class UDialogCommentNode;

/* Custom IDetailsView NotifyHook for custom actions when a property is being updated.  */
class FDialogGraphDetailsNotifyHook : public FNotifyHook
{
public:

	UDialogTree* Tree;
	UDialogGraph* Graph;
	UObject* SelectedDetailsObject;

	FDialogGraphDetailsNotifyHook(UDialogGraph* _Graph, UDialogTree* _Tree);
	virtual void NotifyPreChange(FProperty* PropertyAboutToChange) override;
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged);
};

/* Toolkit for handling DialogEdGraph and it's associated Schema.  Implements most of the heavy lifting tasks for the graph
 * including serializing the graph data into runtime data for the DialogTree asset we're editing. 
 */
class FDialogTreeEditorToolkit : public FAssetEditorToolkit, public FEditorUndoClient
{

public:
	void Initialize(UDialogTree* InDialogTree, const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost);

	TSharedPtr<FDialogGraphDetailsNotifyHook> HookInstance;

	FName GetToolkitFName() const override;
	FText GetBaseToolkitName() const override;
	FString GetWorldCentricTabPrefix() const override;
	FLinearColor GetWorldCentricTabColorScale() const override;
	bool bGraphInitialised = false;


	void UpdateDialogTreeData();
	void HandleCreateComment();
	void HandleCopySelectedNodes(FGraphPanelSelectionSet Nodes);
	void HandlePasteNodes();
	void HandleCutSelectedNodes(FGraphPanelSelectionSet Nodes);
	void HandleDuplicateNodes(FGraphPanelSelectionSet Nodes);

	void OnClose() override;

	// Undo client interface
	virtual void PostUndo(bool bSuccess) override;

private:


	/* Delegates */
	FDelegateHandle GraphChangedDelegateHandle;
	void OnGraphChanged(const FEdGraphEditAction& ActionType);
	FDelegateHandle NodeDeletedHandle;
	void OnNodeDeletedFromGraph(const FGuid& Id);
	FDelegateHandle OnSelectedNode;
	void OnNodeSelected(UEdGraphNode* Node);

	UDialogTree* DialogTree;
	TSharedPtr<IDetailsView> DetailsView;
	TSharedPtr<SDialogGraphEditor> GraphEditorWidget;
	class UDialogGraph* EditorGraph;
	const class UDialogGraphSchema* GraphSchema;

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	void OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged);

	TSharedRef<SDockTab> SpawnGraphTab(const FSpawnTabArgs& Args);
	/*Rebuilds the node graph from the asset the graph is representing.  Called when the graph is loaded. */
	void BuildNodeGraphFromAssetData();
	/* Adds a custom button to the toolbar for viewing the dialog tree asset data */
	void AddToolbarButtons(FToolBarBuilder& ToolbarBuilder);	
	void OnShowAssetDetails()
	{
		SetDetailsViewMode(EMenuMode::StatesOnly);

		if (DetailsView.IsValid() && DialogTree)
		{
			DetailsView->SetObject( (UObject*) DialogTree);
		}
	}

	void SetDetailsViewMode(EMenuMode Mode);

	void SetDialogEventNodeDetailsCustomization(UDialogEventNodeBase* Node);
	EEventDetailsMode GetModeForEventNode(UDialogEventNodeBase* Node);

	void SetDialogConditionNodeDetailsCustomization(UDialogConditionsNode* Node);
	EConditionDetailsMode GetModeForConditionNode(UDialogConditionsNode* Node);

	/* Helper function to filter classes into appropriate arrays based on their class. part of serializing process.*/
	void FilterNodeByClass(UEdGraphNode* Node, TArray<UDialogGraphNode*>& DialogNodes, TArray<UPlayerChoiceNode*>& PlayerChoiceNodes, TArray<UDialogEventNodeBase*>& EventNodes, TArray<UDialogConditionsNode*>& ConditionNodes);
	/* Serialize all dialog sequence nodes into the DialogTree Sequence map */
	void SerializeDialogNodes(TArray<UDialogGraphNode*> DialogNodes);
	/* Serialize all dialog choice nodes into the DialogTree DialogChoices map */
	void SerializePlayerChoiceNodes(TArray<UPlayerChoiceNode*>& PlayerChoiceNodes);
	/* Serialize all Condition nodes into the DialogTree Conditions map */
	void SerializeConditionNodes(TArray<UDialogConditionsNode*>& ConditionNodes);
	/* Serialize all Event nodes into the DialogTree Events map */
	void SerializeEventNodes(TArray<UDialogEventNodeBase*> EventNodes);

	/* Helper for serializing Condition and event SubNodes. Gets the GUID's of all embedded condition nodes, serializes them
	   and passes back the Id's of the subnodes. */
	TArray<FGuid> ParseConditionSubNodesOnNode(UEdGraphNode* Node, FName PinName);
	/* Helper for serializing Condition and event SubNodes. Gets the GUID's of all embedded Event nodes, serializes them
	   and passes back the Id's of the subnodes. */
	TArray<FGuid> ParseEventSubNodesOnNode(UEdGraphNode* Node, FName PinName);

};