// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_LobbyPlayerRow.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PlayerState/MGLobbyPlayerState.h"
#include "Type/MGPlayerColor.h"

void UUW_LobbyPlayerRow::Setup(AMGLobbyPlayerState* PS)
{
	if (PS->bIsReady)
	{
		ReadyCheck->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		ReadyCheck->SetVisibility(ESlateVisibility::Collapsed);
	}

	// 이름
	PlayerNameText->SetText(FText::FromString(PS->GetPlayerName()));

	// 색 (공용 변환 사용)
	const FLinearColor Color = MGPlayerColorToLinear(PS->PlayerColor);
	ColorSwatch->SetColorAndOpacity(Color);

	// 레디
	ReadyText->SetText(PS->bIsReady ? NSLOCTEXT("Lobby", "Ready", "READY") : NSLOCTEXT("Lobby", "NotReady", "..."));
}
