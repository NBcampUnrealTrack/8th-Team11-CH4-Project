// UW_PassBombLayout.cpp

#include "UI/UW_PassBombLayout.h"
#include "Components/TextBlock.h"
#include "GameState/MGPassBombGameState.h"

void UUW_PassBombLayout::UpdateBombHolderText(const FString& NewHolderName)
{
	if (IsValid(BombHolderText))
	{
		// 받아온 문자열을 화면 텍스트에 적용
		BombHolderText->SetText(FText::FromString(NewHolderName));
	}
}

void UUW_PassBombLayout::UpdateBombTimerText(int32 RemainTime)
{
	if (IsValid(BombTimerText))
	{
		FString TimeString = FString::Printf(TEXT("%d"), RemainTime);
		BombTimerText->SetText(FText::FromString(TimeString));
	}
}

void UUW_PassBombLayout::UpdateAliveCountText(int32 AliveCount, int32 TotalCount)
{
	if (IsValid(AliveCountText))
	{
		FString FormatText = FString::Printf(TEXT("생존자 : %d / %d"), AliveCount, TotalCount);
		AliveCountText->SetText(FText::FromString(FormatText));
	}
}
