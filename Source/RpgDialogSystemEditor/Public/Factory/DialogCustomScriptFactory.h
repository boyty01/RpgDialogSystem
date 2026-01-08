// Copyright DMTesseracT LTd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Script/DialogScriptEventBase.h"
#include "DialogCustomScriptFactory.generated.h"


UCLASS()
class UDialogCustomScriptFactory : public UFactory
{
    GENERATED_BODY()

public:

    UDialogCustomScriptFactory()
    {
        bCreateNew = true;
        bEditAfterNew = true;
        SupportedClass = UDialogScriptEventBase::StaticClass();
    }

   
    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
        EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

    virtual FText GetDisplayName() const override
    {
        return FText::FromString("Dialog Event Script");
    }
};