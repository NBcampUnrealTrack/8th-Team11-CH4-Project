// UW_FinalResultRow.cpp


#include "UI/UW_FinalResultRow.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PlayerState/MGPlayerState.h"

void UUW_FinalResultRow::Setup(AMGPlayerState* PS, int32 Rank)
{
	if (ensure(IsValid(PS)) == false)
	{
		return;
	}

	RankText->SetText(FText::AsNumber(Rank));

	FLinearColor BadgeColor;
	switch (Rank)
	{
	case 1:  { BadgeColor = FLinearColor(1.00f, 0.84f, 0.00f); break; } // 금
	case 2:  { BadgeColor = FLinearColor(0.75f, 0.75f, 0.78f); break; } // 은
	case 3:  { BadgeColor = FLinearColor(0.80f, 0.50f, 0.20f); break; } // 동
	default: { BadgeColor = FLinearColor(0.80f, 0.80f, 0.82f); break; } // 회색
	}
	RankBadge->SetColorAndOpacity(BadgeColor);
	
	ColorSwatch->SetColorAndOpacity(PS->GetPlayerLinearColor());

	PlayerNameText->SetText(FText::FromString(PS->GetPlayerName()));

	UTextBlock* RoundTexts[3] = { Round1Text, Round2Text, Round3Text };
	for (int32 i = 0; i < 3; ++i)
	{
		if (PS->RoundScores.IsValidIndex(i))
		{
			RoundTexts[i]->SetText(FText::AsNumber(PS->RoundScores[i]));
		}
		else
		{
			RoundTexts[i]->SetText(FText::FromString(TEXT("-")));
		}
	}

	TotalText->SetText(FText::AsNumber(PS->TotalScore));
}
