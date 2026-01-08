// Copyright DMTesseracT LTd. All rights reserved.


#include "Factory/DialogNodeFactory.h"
#include "Nodes/DialogGraphNode.h"
#include "Nodes/PlayerChoiceNode.h"
#include "SGraphNodeComment.h"
#include "Slate/PlayerChoiceNodeStyle.h"
#include "Nodes/QuestConditionNode.h"
#include "Nodes/InventoryConditionNode.h"
#include "Nodes/LocalStateConditionNode.h"
#include "Nodes/DialogInventoryEventNode.h"
#include "Nodes/DialogQuestEventNode.h"
#include "Nodes/DialogStateEventNode.h"
#include "Nodes/DialogCommentNode.h"
#include "Nodes/DialogCustomEventNode.h"
#include "Slate/DialogConditionNodeStyle.h"
#include "Slate/DialogEventNodeStyle.h"

TSharedPtr<SGraphNode> FDialogNodeFactory::CreateNode(UEdGraphNode* InNode) const
{
    if(!InNode) return nullptr;

    if (UDialogGraphNode* DialogNode = Cast<UDialogGraphNode>(InNode))
    {
         return SNew(SDialogNodeStyle).GraphNode(DialogNode);
    }

    if (UPlayerChoiceNode* PlayerChoiceNode = Cast<UPlayerChoiceNode>(InNode))
    {
        return SNew(SPlayerChoiceNodeStyle).GraphNode(PlayerChoiceNode);
    }

    if (UQuestConditionNode* ConditionNode = Cast<UQuestConditionNode>(InNode))
    {
        return SNew(SDialogConditionsNodeStyle).GraphNode(ConditionNode);
    }

    if (UInventoryConditionNode* InvConditionNode = Cast<UInventoryConditionNode>(InNode))
    {
        return SNew(SDialogConditionsNodeStyle).GraphNode(InvConditionNode);
    }

    if (UDialogCommentNode* CommentNode = Cast< UDialogCommentNode>(InNode))
    {
       return SNew(SGraphNodeComment, CommentNode);       
    }

    if (ULocalStateConditionNode* StateConditionNode = Cast<ULocalStateConditionNode>(InNode))
    {
        return SNew(SDialogConditionsNodeStyle).GraphNode(StateConditionNode);
    }

    if (UDialogInventoryEventNode* InventoryEventNode = Cast<UDialogInventoryEventNode>(InNode))
    {
        return SNew(SDialogEventNodeStyle).GraphNode(InventoryEventNode);
    }

    if (UDialogQuestEventNode* QuestEventNode = Cast<UDialogQuestEventNode>(InNode))
    {
        return SNew(SDialogEventNodeStyle).GraphNode(QuestEventNode);
    }

    if (UDialogStateEventNode* StateEventNode = Cast<UDialogStateEventNode>(InNode))
    {
        return SNew(SDialogEventNodeStyle).GraphNode(StateEventNode);
    }

    if(UDialogCustomEventNode* CustomEventNode = Cast<UDialogCustomEventNode>(InNode))
    {
        return SNew(SDialogEventNodeStyle).GraphNode(CustomEventNode);
	}
    return nullptr;
}
