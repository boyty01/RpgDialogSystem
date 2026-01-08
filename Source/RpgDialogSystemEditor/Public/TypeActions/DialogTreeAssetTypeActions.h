// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "AssetDefinitionDefault.h"
#include "Script/DialogScriptEventBase.h"
#include "DataAsset/DialogTree.h"
#include "DialogTreeAssetTypeActions.generated.h"
/**
 * 
 */


/*
class RPGDIALOGSYSTEMEDITOR_API FDialogTreeAssetTypeActions : public FAssetTypeActions_Base
{
public:
    FDialogTreeAssetTypeActions(EAssetTypeCategories::Type InAssetCategory)
        : AssetCategory(InAssetCategory) {
    }
    // The name of the asset type (e.g. DialogTree)
    virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "FDialogTreeAssetTypeActions", "Dialog Tree"); }

    virtual FColor GetTypeColor() const override { return FColor(180, 90, 255); }

    virtual UClass* GetSupportedClass() const override;

    virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor) override;

    virtual uint32 GetCategories() override { return AssetCategory; }

private:
    EAssetTypeCategories::Type AssetCategory;

};
*/

UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogTreeAssetDefinition : public UAssetDefinitionDefault
{
    GENERATED_BODY()

public:

    // Display properties
    virtual FText GetAssetDisplayName() const override
    {
        return NSLOCTEXT("AssetDefinition", "AssetDefinition_DialogTree", "Dialog Tree");
    };

    virtual FLinearColor GetAssetColor() const override
    {
        return FColor(180, 90, 255);
    };

    virtual TSoftClassPtr<> GetAssetClass() const override
    {
        return UDialogTree::StaticClass();
    }

    virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override
    {
        static const TArray Categories =
        {
            FAssetCategoryPath(FText::FromString("RPG Dialog"))
        };

        return Categories;
    };

    virtual EAssetCommandResult OpenAssets(const FAssetOpenArgs& OpenArgs) const override;
};


UCLASS()
class RPGDIALOGSYSTEMEDITOR_API UDialogCustomScriptAssetDefinition : public UAssetDefinitionDefault
{
    GENERATED_BODY()

public:

    // Display properties
    virtual FText GetAssetDisplayName() const override
    {
        return NSLOCTEXT("AssetDefinition", "AssetDefinition_DialogCustomScript", "Dialog Custom Script");
    };

    virtual FLinearColor GetAssetColor() const override
    {
        return FColor(0, 128, 255);
    };

    virtual TSoftClassPtr<> GetAssetClass() const override
    {
        return UDialogScriptEventBase::StaticClass();
    }

    virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override
    {
        static const TArray Categories =
        {
            FAssetCategoryPath(FText::FromString("RPG Dialog"))
        };

        return Categories;
    };

};