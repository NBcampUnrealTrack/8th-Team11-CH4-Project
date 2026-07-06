// UW_PassBombLayout.cpp

#include "UI/UW_PassBombLayout.h"
#include "Components/TextBlock.h"

void UUW_PassBombLayout::UpdateBombHolderText(const FString& NewHolderName)
{
	if (IsValid(BombHolderText))
	{
		// 받아온 문자열을 화면 텍스트에 적용
		BombHolderText->SetText(FText::FromString(NewHolderName));
	}
}
