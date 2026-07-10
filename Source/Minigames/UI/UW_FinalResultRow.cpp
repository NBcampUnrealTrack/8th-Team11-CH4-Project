// UW_FinalResultRow.cpp


#include "UI/UW_FinalResultRow.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PlayerState/MGLobbyPlayerState.h"

// UW_FinalResultRow.cpp


#include "UI/UW_FinalResultRow.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PlayerState/MGPlayerState.h"

void UUW_FinalResultRow::Setup(AMGPlayerState* PS, int32 Rank)
{
	if (IsValid(PS) == false)
	{
		return;
	}

	RankText->SetText(FText::AsNumber(Rank));

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
