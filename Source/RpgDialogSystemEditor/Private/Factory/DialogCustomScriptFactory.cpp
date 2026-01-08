// Copyright DMTesseracT LTd. All rights reserved.

#include "Factory/DialogCustomScriptFactory.h"
#include "Kismet2/KismetEditorUtilities.h"

UObject* UDialogCustomScriptFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    
    return FKismetEditorUtilities::CreateBlueprint(
        UDialogScriptEventBase::StaticClass(), // Parent class
        InParent,
        Name,
        BPTYPE_Normal,
        UBlueprint::StaticClass(),
        UBlueprintGeneratedClass::StaticClass()
    );
   
  //  return NewObject<UDialogScriptEventBase>(InParent, Class, Name, Flags);
}
