// Copyright DMTesseracT LTd. All rights reserved.

#pragma once
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SCompoundWidget.h"
#include "EdGraph/EdGraph.h"



DECLARE_DELEGATE(FOnRequestCreateComment);
DECLARE_DELEGATE(FOnRequestCreateNode);
DECLARE_DELEGATE_OneParam(FOnRequestCopyNode, FGraphPanelSelectionSet Nodes);
DECLARE_DELEGATE_OneParam(FOnRequestCutNode, FGraphPanelSelectionSet Nodes);
DECLARE_DELEGATE(FOnRequestPasteNode);
DECLARE_DELEGATE_OneParam(FOnRequestDuplicateNode, FGraphPanelSelectionSet Nodes);


class SDialogGraphEditor : public SCompoundWidget {
public:
    SLATE_BEGIN_ARGS(SDialogGraphEditor) {}
        SLATE_ARGUMENT(UEdGraph*, GraphToEdit)
        SLATE_ARGUMENT(TSharedPtr<IDetailsView>, DetailsView)
		SLATE_ARGUMENT(SGraphEditor::FGraphEditorEvents, GraphEvents)
        SLATE_ARGUMENT(FGraphAppearanceInfo, Appearance)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    UEdGraph* Graph;
    TSharedPtr<IDetailsView> DetailsView;
    TSharedPtr<SGraphEditor> GraphEditor;
    TSharedPtr<IDetailsView> GraphDetailsView;
    TSharedPtr<FUICommandList> GraphEditorCommands;

    TSet<UObject*> LastSelection;

    FOnRequestCreateComment OnRequestCreateComment;
	FOnRequestCopyNode OnRequestCopyNode;
	FOnRequestCutNode OnRequestCutNode;
	FOnRequestPasteNode OnRequestPasteNode;
	FOnRequestDuplicateNode OnRequestDuplicateNode;

    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;



private:

    void OnSelectionChanged(const TSet<UObject*>& SelectedNodes);
    bool CanDeleteSelectedNodes() const;
    void OnDeleteSelectedNodes();
    void OnCreateComment();
    bool CanCreateComment() { return true; };
	void OnCopySelectedNodes();
    bool CanCopyNodes() { return true; };
	void OnCutSelectedNodes();
	bool CanCutNodes() { return true; };
	void OnPasteNodes();
	bool CanPasteNodes() const { return true; };
	void OnDuplicateNodes();
	bool CanDuplicateNodes() const { return true; };
};
