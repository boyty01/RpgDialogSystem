// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAsset/DialogTree.h"
#include "DialogTreeFactory.generated.h"

UCLASS()
class UDialogTreeFactory : public UFactory
{
    GENERATED_BODY()

public:

    UDialogTreeFactory()
    {
        bCreateNew = true;
        bEditAfterNew = true;
        SupportedClass = UDialogTree::StaticClass();
    }

    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
        EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override
    {
        return NewObject<UDialogTree>(InParent, Class, Name, Flags);
    }
};