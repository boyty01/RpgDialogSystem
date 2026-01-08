#include "RpgDialogSlateStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/Paths.h"

TSharedPtr<FSlateStyleSet> FRpgDialogSlateStyle::StyleInstance = nullptr;

void FRpgDialogSlateStyle::Initialize()
{
    if (!StyleInstance.IsValid())
    {
        Create();
        FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
    }
}

void FRpgDialogSlateStyle::Shutdown()
{
    if (StyleInstance.IsValid())
    {
        FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
        ensure(StyleInstance.IsUnique());
        StyleInstance.Reset();
    }
}

FName FRpgDialogSlateStyle::GetStyleSetName()
{
    return FName("RpgDialogSlateStyle");
}

const ISlateStyle& FRpgDialogSlateStyle::Get()
{
    return *StyleInstance;
}

void FRpgDialogSlateStyle::Create()
{
    // Name of the style set
    StyleInstance = MakeShareable(new FSlateStyleSet(GetStyleSetName()));

    FString BaseDir = IPluginManager::Get().FindPlugin("RpgDialogSystem")->GetBaseDir();
    StyleInstance->SetContentRoot(BaseDir / TEXT("Resources"));

    // Player choice icon
    StyleInstance->Set(
        "RpgDialog.SplitPath",
        new FSlateImageBrush(
            StyleInstance->RootToContentDir(TEXT("Icons/path-icon.png")),
            FVector2D(48.f, 48.f)
        )
    );
    // Dialog node icon 
    StyleInstance->Set(
        "RpgDialog.Speech",
        new FSlateImageBrush(
            StyleInstance->RootToContentDir(TEXT("Icons/voice-icon.png")),
            FVector2D(48.f, 48.f)
        )
    );
}
