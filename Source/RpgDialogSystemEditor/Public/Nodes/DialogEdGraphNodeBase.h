// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "DialogTypes.h"
#include "Interface/RpgDialogNodeInterface.h"
#include "DialogEdGraphNodeBase.generated.h"

UENUM()
enum class EDialogGraphConfigurationState : uint8
{
	Valid,
	Warning,
	Error
};


DECLARE_MULTICAST_DELEGATE_OneParam(FOnNodeDeleted, UDialogEdGraphNodeBase*);
/**
 * 
 */
UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogEdGraphNodeBase : public UEdGraphNode, public IRpgDialogNodeInterface
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	EDialogGraphConfigurationState ConfigurationState{ EDialogGraphConfigurationState::Valid };

	UDialogEdGraphNodeBase()
	{
		SetFlags(RF_Transactional);

		ValidateNodeConfiguration();
	}

	FOnNodeDeleted OnNodeDeletedDelegate;

	virtual void PostPasteNode() override;

	/* Gets the calculated screen size of this nodes slate element */
	virtual FVector2D GetNodeSize();

	UPROPERTY()
	bool bIsSubNode{ false };

	UPROPERTY()
	UDialogEdGraphNodeBase* ParentNode;

	/* an Identifiable title for the node in the node graph. */
	UPROPERTY(EditAnywhere, Category="Editor")
	FText NodeTitle;

	/*Specify the pin colour for the output pin from this node. Useful for helping visualise a busy busy graph. */
	UPROPERTY(EditAnywhere, Category="Editor")
	FLinearColor OutPinColour{FLinearColor::White};
	
	/* Specify the pin line width for the output pin from this node. Useful for helping visualise a busy node graph. */
	UPROPERTY(EditAnywhere, Category = "Editor")
	float OutPinLineWidth{ 1.f };

	UPROPERTY()
	TArray<UDialogEdGraphNodeBase*> ChildNodes;

	virtual void OnSubNodeDeleted(UDialogEdGraphNodeBase* SubNode);

	virtual FPinConnectionResponse CanLinkTo(const UEdGraphPin* MyOutputPin, const UEdGraphPin* OtherInputPin) const override
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("PIN LOGIC NOT IMPLEMENTED"));
	}

	bool bShowAttributesOnNode{ false };

	bool GetShowAttributesOnNode() const override
	{
		return bShowAttributesOnNode;
	}

	FScopedTransaction* PendingTransaction;

	virtual bool CanUserDeleteNode() const override { return true; };
	virtual void OnNodeMoved();

	virtual void PreEditChange(FProperty* PropertyAboutToChange) override
	{
		Super::PreEditChange(PropertyAboutToChange);

	}

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);

		if (PendingTransaction)
		{
			delete PendingTransaction;
			PendingTransaction = nullptr;
		}

		if (GetGraph())
			GetGraph()->NotifyGraphChanged();
	}

	virtual void ValidateNodeConfiguration() {};

	void DestroyNode() override;

	// Add a child node to this node.
	virtual void AddNode(UDialogEdGraphNodeBase* Node) 
	{
		ChildNodes.Add(Node);
		Node->ParentNode = this;
		Node->OnNodeDeletedDelegate.AddUObject(this, &UDialogEdGraphNodeBase::OnSubNodeDeleted);
	};
	
	/*Templated getter for subnodes of specified type. */
	template <typename T>
	TArray<T*> GetSubNodesOfType()
	{
		static_assert (TIsDerivedFrom<T, UDialogEdGraphNodeBase>::IsDerived,
			"T Must derive from UDialogEdGraphNodeBase");

		TArray<T*> OutArray;

		for (auto& Node : ChildNodes)
		{
			if(T* TypedNode = Cast<T>(Node))
			{
				OutArray.Add(TypedNode);
			}
		}
		return OutArray;
	};

	void DebugLogPostPaste(int32 Depth = 0) const;
	virtual void ValidateSubNodes() {};
};

