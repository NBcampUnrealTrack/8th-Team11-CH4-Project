// UW_FlagLayout.cpp


#include "UI/UW_FlagLayout.h"
#include "Components/TextBlock.h"

void UUW_FlagLayout::UpdateFlagHolderText(const FString& NewHolderName)
{
	if (IsValid(FlagHolderText))
	{
		LastFlagHolder = FlagHolder;
		FlagHolder = FText::FromString(NewHolderName);
		FlagHolderText->SetText(FText::Format(FText::FromString(TEXT("{0} 보유 중")), FlagHolder));
	}
}

void UUW_FlagLayout::UpdateFlagTimerText(int32 RemainTime)
{
	if (IsValid(FlagTimerText))
	{
		int32 Minutes = RemainTime / 60;
		int32 Seconds = RemainTime % 60;
		FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		FlagTimerText->SetText(FText::FromString(TimeString));
	}
}
