#pragma once

#include "Styling/SlateStyle.h"

class FRpgDialogSlateStyle
{
public:

    // Creates + registers the style
    static void Initialize();

    // Destructs + unregisters the style
    static void Shutdown();

    // Quick access to the style object
    static const ISlateStyle& Get();

    // Unique style set name
    static FName GetStyleSetName();

private:
    static TSharedPtr<FSlateStyleSet> StyleInstance;

    static void Create();
};