#pragma once

#include "IDetailCustomization.h"

enum class EMenuMode
{
	StatesOnly,
	NoStates,
	Everything
};


class FDialogTreeCustomDetail : public IDetailCustomization
{
public:


	static TSharedRef<IDetailCustomization> MakeInstance(EMenuMode InMenuMode);

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	EMenuMode MenuMode;

	// Private constructor used through MakeInstance
	FDialogTreeCustomDetail(EMenuMode InMenuMode)
		: MenuMode(InMenuMode)
	{
	}
};