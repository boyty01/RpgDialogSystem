#pragma once

#include "IDetailCustomization.h"

enum class EEventDetailsMode
{
	StatesView,
	QuestView,
	InventoryView,
	CustomScriptView,
	Everything
};


class FEventNodeCustomDetail : public IDetailCustomization
{
public:


	static TSharedRef<IDetailCustomization> MakeInstance(EEventDetailsMode  InEventDetailsMode);

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	EEventDetailsMode EventDetailsMode;

	// Private constructor used through MakeInstance
	FEventNodeCustomDetail(EEventDetailsMode InDetailsMode)
		: EventDetailsMode(InDetailsMode)
	{
	}
};