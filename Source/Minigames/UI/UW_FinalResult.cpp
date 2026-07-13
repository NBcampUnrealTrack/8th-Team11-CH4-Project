// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_FinalResult.h"

#include "UI/UW_FinalResultRow.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Controller/MGPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "PlayerState/MGPlayerState.h"

void UUW_FinalResult::NativeConstruct()
{
	Super::NativeConstruct();

	ReturnButton->OnClicked.AddDynamic(this, &ThisClass::OnReturnClicked);

	// bReadyToReturn / Rank 가 Replicated라 늦게 도착 → 주기적으로 갱신
	RefreshBoard();
	GetWorld()->GetTimerManager().SetTimer(
		RefreshTimerHandle,
		this,
		&ThisClass::RefreshBoard,
		0.3f,
		true
	);
}

void UUW_FinalResult::RefreshBoard()
{
	AGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	if (IsValid(GS) == false || PlayerRowClass == nullptr)
	{
		return;
	}

	TArray<AMGPlayerState*> Players;
	for (APlayerState* PS : GS->PlayerArray)
	{
		if (AMGPlayerState* MGPS = Cast<AMGPlayerState>(PS))
		{
			Players.Add(MGPS);
		}
	}
	Players.Sort([](const AMGPlayerState& A, const AMGPlayerState& B)
	{
		return A.TotalScore > B.TotalScore;
	});

	int32 ReadyCount = 0;
	for (int32 i = 0; i < Players.Num(); ++i)
	{
		if (Players[i]->bReadyToReturn)
		{
			++ReadyCount;
		}

		if (RowWidgets.IsValidIndex(i) == false)
		{
			UUW_FinalResultRow* Row = CreateWidget<UUW_FinalResultRow>(this, PlayerRowClass);
			PlayerListBox->AddChild(Row);
			RowWidgets.Add(Row);
		}
		RowWidgets[i]->Setup(Players[i], i + 1);
	}

	// 남는 줄 제거 (뒤에서부터)
	for (int32 i = RowWidgets.Num() - 1; i >= Players.Num(); --i)
	{
		RowWidgets[i]->RemoveFromParent();
		RowWidgets.RemoveAt(i);
	}

	ReadyCountText->SetText(FText::Format(
		NSLOCTEXT("FinalResult", "ReadyToReturn", "돌아가기 {0}/{1}"),
		FText::AsNumber(ReadyCount),
		FText::AsNumber(Players.Num()))
	);
}

void UUW_FinalResult::OnReturnClicked()
{
	if (AMGPlayerController* PC = GetOwningPlayer<AMGPlayerController>())
	{
		PC->ServerRPC_ReadyToReturn();
	}
	ReturnButton->SetIsEnabled(false);
}

void UUW_FinalResult::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RefreshTimerHandle);
	}
	Super::NativeDestruct();
}
