#pragma once

#include "IDetailCustomization.h"

enum class EConditionDetailsMode
{
	StatesView,
	QuestView,
	InventoryView,
	Everything
};


class FConditionNodeCustomDetail : public IDetailCustomization
{
public:


	static TSharedRef<IDetailCustomization> MakeInstance(EConditionDetailsMode  InEventDetailsMode);

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	EConditionDetailsMode ConditionDetailsMode;

	// Private constructor used through MakeInstance
	FConditionNodeCustomDetail(EConditionDetailsMode InDetailsMode)
		: ConditionDetailsMode(InDetailsMode)
	{
	}
};