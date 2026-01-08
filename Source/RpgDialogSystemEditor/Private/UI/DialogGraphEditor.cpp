// Copyright DMTesseracT LTd. All rights reserved.


#include "UI/DialogGraphEditor.h"
#include "GraphEditor.h"
#include "Framework/Commands/GenericCommands.h"
#include "EdGraph/EdGraph.h"
#include "DialogGraphEditorCommands.h"
#include <Graph/DialogGraph.h>
#include "HAL/PlatformApplicationMisc.h"  
#include "EdGraphUtilities.h"              
#include "ScopedTransaction.h"     


void SDialogGraphEditor::Construct(const FArguments& InArgs)
{
    Graph = InArgs._GraphToEdit;
    GraphDetailsView = InArgs._DetailsView;

    GraphEditorCommands = MakeShareable(new FUICommandList);
    GraphEditorCommands->MapAction(
        FGenericCommands::Get().Delete, 
        FExecuteAction::CreateSP(this, &SDialogGraphEditor::OnDeleteSelectedNodes),
        FCanExecuteAction::CreateSP(this, &SDialogGraphEditor::CanDeleteSelectedNodes)
    );

    GraphEditorCommands->MapAction(
        FDialogGraphEditorCommands::Get().CreateComment,
        FExecuteAction::CreateSP(this, &SDialogGraphEditor::OnCreateComment),
        FCanExecuteAction::CreateSP(this, &SDialogGraphEditor::CanCreateComment)
    );

    // Copy
    GraphEditorCommands->MapAction(
        FDialogGraphEditorCommands::Get().CopyNodes,
        FExecuteAction::CreateSP(this, &SDialogGraphEditor::OnCopySelectedNodes),
        FCanExecuteAction::CreateSP(this, &SDialogGraphEditor::CanCopyNodes));

    // Cut
    GraphEditorCommands->MapAction(
        FDialogGraphEditorCommands::Get().CutNodes,
        FExecuteAction::CreateSP(this, &SDialogGraphEditor::OnCutSelectedNodes),
        FCanExecuteAction::CreateSP(this, &SDialogGraphEditor::CanCutNodes)
    );

    // Paste
    GraphEditorCommands->MapAction(
        FDialogGraphEditorCommands::Get().PasteNodes,
        FExecuteAction::CreateSP(this, &SDialogGraphEditor::OnPasteNodes),
        FCanExecuteAction::CreateSP(this, &SDialogGraphEditor::CanPasteNodes)
    );

    // Duplicate
    GraphEditorCommands->MapAction(
        FDialogGraphEditorCommands::Get().DuplicateNodes,
        FExecuteAction::CreateSP(this, &SDialogGraphEditor::OnDuplicateNodes),
        FCanExecuteAction::CreateSP(this, &SDialogGraphEditor::CanDuplicateNodes)
    );

   

    GraphEditor = SNew(SGraphEditor)
        .AdditionalCommands(GraphEditorCommands)
        .GraphToEdit(Graph)   
		.GraphEvents(InArgs._GraphEvents)
        .Appearance(InArgs._Appearance)
        // bind selection etc.
        ;

    ChildSlot
        [
            SNew(SSplitter)
                + SSplitter::Slot()
                .Value(0.7f)
                [
                    GraphEditor.ToSharedRef()
                ]
                + SSplitter::Slot()
                .Value(0.3f)
                [
                    GraphDetailsView.IsValid() ? GraphDetailsView.ToSharedRef() : SNullWidget::NullWidget
                ]
        ];


}

void SDialogGraphEditor::OnCopySelectedNodes()
{
	OnRequestCopyNode.ExecuteIfBound(GraphEditor->GetSelectedNodes());

	UE_LOG(LogTemp, Log, TEXT("Copying selected nodes"));
    const FGraphPanelSelectionSet& SelectedNodes = GraphEditor->GetSelectedNodes();

    if (SelectedNodes.Num() == 0)
        return;

    // Export selected nodes to text
    FString ExportedText;
    FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);

    // Copy to clipboard
    FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

void SDialogGraphEditor::OnCutSelectedNodes()
{
	UE_LOG(LogTemp, Log, TEXT("Cutting selected nodes"));
    OnRequestCutNode.ExecuteIfBound(GraphEditor->GetSelectedNodes());

    OnCopySelectedNodes();
    OnDeleteSelectedNodes();
}

void SDialogGraphEditor::OnPasteNodes()
{
    OnRequestPasteNode.ExecuteIfBound();

    if (!GraphEditor.IsValid())
        return;

    if (!Graph)
        return;

    // Get text from clipboard
    FString TextToImport;
    FPlatformApplicationMisc::ClipboardPaste(TextToImport);

    // Check if we can import
    if (!FEdGraphUtilities::CanImportNodesFromText(Graph, TextToImport))
        return;

    // Clear current selection
    GraphEditor->ClearSelectionSet();

    // Import the nodes
    TSet<UEdGraphNode*> PastedNodes;
    FEdGraphUtilities::ImportNodesFromText(Graph, TextToImport, PastedNodes);

    // Position pasted nodes at mouse cursor or offset from originals
    FVector2f PasteLocation = GraphEditor->GetPasteLocation2f();

    FVector2f AvgNodePosition(0.0f, 0.0f);
    for (UEdGraphNode* Node : PastedNodes)
    {
        AvgNodePosition.X += Node->NodePosX;
        AvgNodePosition.Y += Node->NodePosY;
       
        if (UDialogEdGraphNodeBase* DialogNode = Cast<UDialogEdGraphNodeBase>(Node))
        {
            DialogNode->DebugLogPostPaste();
        }
    }

    if (PastedNodes.Num() > 0)
    {
        AvgNodePosition /= PastedNodes.Num();
    }

    // Offset nodes to paste location
    for (UEdGraphNode* Node : PastedNodes)
    {
        Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + PasteLocation.X;
        Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + PasteLocation.Y;

        Node->SnapToGrid(16);

        // Select the pasted node
        GraphEditor->SetNodeSelection(Node, true);
    }

    // Notify graph changed
    Graph->NotifyGraphChanged();

    // Update UI
    GraphEditor->NotifyGraphChanged();
}

void SDialogGraphEditor::OnDuplicateNodes()
{
    OnRequestDuplicateNode.ExecuteIfBound(GraphEditor->GetSelectedNodes());

    OnCopySelectedNodes();
    OnPasteNodes();
}


void SDialogGraphEditor::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    if (!GraphEditor.IsValid() || !GraphDetailsView.IsValid())
        return;

    TSet<UObject*> CurrentSelection = GraphEditor->GetSelectedNodes();

    if (!CurrentSelection.Includes(LastSelection) || !LastSelection.Includes(CurrentSelection))
    {
        LastSelection = CurrentSelection;
        OnSelectionChanged(CurrentSelection);
    }
}

void SDialogGraphEditor::OnSelectionChanged(const TSet<UObject*>& SelectedNodes)
{
    if (SelectedNodes.Num() == 1)
    {
        UObject* SelectedNode = *SelectedNodes.CreateConstIterator();
        GraphDetailsView->SetObject(SelectedNode);
        
        UDialogGraph* DialogGraph = Cast<UDialogGraph>(Graph);
        UEdGraphNode* AsGraphNode = Cast<UEdGraphNode>(SelectedNode);
        
        if(DialogGraph && AsGraphNode)
            Cast<UDialogGraph>(Graph)->OnSelectedNode(AsGraphNode);
        
    }
    else
    {
        GraphDetailsView->SetObject(nullptr);
    }
}


bool SDialogGraphEditor::CanDeleteSelectedNodes() const
{
    return  GraphEditor.IsValid() && GraphEditor->GetSelectedNodes().Num() > 0;
}

void SDialogGraphEditor::OnDeleteSelectedNodes()
{
    if (GraphEditor.IsValid())
    {
        const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

        FScopedTransaction Transaction(FText::FromString("Delete Node(s)"));
        for (UObject* NodeObj : SelectedNodes)
        {
            if (UEdGraphNode* Node = Cast<UEdGraphNode>(NodeObj))
            {
                if (Node->CanUserDeleteNode())
                {
                    Graph->Modify();
                    Node->Modify();
                    if (UDialogEdGraphNodeBase* AsDialogNode = Cast<UDialogEdGraphNodeBase>(Node))
                    {
                        if (AsDialogNode->bIsSubNode)
                        {
                            if (AsDialogNode->ParentNode)
                            {
                                AsDialogNode->ParentNode->Modify();
                                AsDialogNode->ParentNode->OnSubNodeDeleted(AsDialogNode);
                            }
                        }
                    }
                    Graph->RemoveNode(Node);
                    Cast<UDialogGraph>(Graph)->OnNodeDeleted(Node);
                }
            }
        }
    }
}

void SDialogGraphEditor::OnCreateComment()
{
    if (OnRequestCreateComment.IsBound())
    {
        OnRequestCreateComment.Execute();
    }
}

