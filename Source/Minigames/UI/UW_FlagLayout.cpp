// UW_FlagLayout.cpp


#include "UI/UW_FlagLayout.h"
#include "Components/TextBlock.h"

void UUW_FlagLayout::UpdateFlagHolderText(const FString& NewHolderName)
{
	if (IsValid(FlagHolderText))
	{
		FlagHolderText->SetText(FText::FromString(NewHolderName));
	}
}

void UUW_FlagLayout::UpdateFlagTimerText(int32 RemainTime)
{
	if (IsValid(FlagTimerText))
	{
		FString TimeString = FString::Printf(TEXT("%d"), RemainTime);
		FlagTimerText->SetText(FText::FromString(TimeString));
	}
}
