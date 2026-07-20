// MGGameStateBase.cpp

#include "GameState/MGGameStateBase.h"
#include "GameState/MGPassBombGameState.h"
#include "GameState/MGFlagGameStateBase.h"
#include "BTN/MGButtonGameState.h"

#include "PlayerState/MGPlayerState.h"

#include "Controller/MGPlayerController.h"
#include "Net/UnrealNetwork.h"

#include "GameInstance/MGGameInstance.h"

EMinigameType AMGGameStateBase::GetCurrentMinigameType()
{
	if (Cast<AMGPassBombGameState>(this))
	{
		return EMinigameType::PassBomb;
	}
	if (Cast<AMGFlagGameStateBase>(this))
	{
		return EMinigameType::FlagGame;
	}
	if (Cast<AMGButtonGameState>(this))
	{
		return EMinigameType::ButtonOwnership;
	}

	return EMinigameType::NONE;
}

void AMGGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MatchState);
	DOREPLIFETIME(ThisClass, RoundState);
	DOREPLIFETIME(ThisClass, AliveCharacters);
	DOREPLIFETIME(ThisClass, EndingTimeRemaining);
}

void AMGGameStateBase::OnRep_MatchState()
{
	if (APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController())
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			const bool bShouldBlockMove =
				(MatchState == EMatchState::Entering ||
					MatchState == EMatchState::Waiting ||
					MatchState == EMatchState::Ending);

			if (bShouldBlockMove)
			{
				Pawn->DisableInput(PC);
			}
			else
			{
				Pawn->EnableInput(PC);
			}
		}
	}

	// 게임이 시작(또는 그 이후)됐으면 인트로 숨김.
	// PlayingCutScene/Playing만 보면, 폭탄게임처럼 시작과 동시에 Ending으로 넘어가는 경우
	// 수동 OnRep 시점엔 이미 Ending이라 숨김을 놓침 → Entering/Waiting이 아니면 전부 숨김.
	if (MatchState == EMatchState::PlayingCutScene || MatchState == EMatchState::Playing)
	{
		if (APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController())
		{
			if (AMGPlayerController* MGPC = Cast<AMGPlayerController>(PC))
			{
				MGPC->HideMinigameIntro();
			}
		}
	}

	if (MatchState == EMatchState::Waiting)
	{
		OnWaitingStarted.Broadcast();
	}

	if (MatchState == EMatchState::Playing)
	{
		OnMinigameStarted.Broadcast();
	}

	if (MatchState == EMatchState::Playing || MatchState == EMatchState::Ending)
	{
		if (UMGGameInstance* GI = GetGameInstance<UMGGameInstance>())
		{
			GI->PlayCurrentLevelBGM();
		}
	}
}

void AMGGameStateBase::SetMatchState(EMatchState NewState)
{
	MatchState = NewState;
	if (HasAuthority())
	{
		OnRep_MatchState();
	}

	/*
	if (MatchState == EMatchState::Playing || MatchState == EMatchState::Ending)
	{
		if (UMGGameInstance* GI = GetGameInstance<UMGGameInstance>())
		{
			GI->PlayCurrentLevelBGM();
		}
	}
	*/
}

void AMGGameStateBase::OnRep_EndingTimeRemaining()
{
	OnEndingTimeChanged.Broadcast(EndingTimeRemaining);
}

TArray<AMGPlayerState*> AMGGameStateBase::GetSortedPlayerStatesByTotalScore()
{ 
	TArray<AMGPlayerState*> SortedPlayers;

	for (APlayerState* PS : PlayerArray)
	{
		if (AMGPlayerState* MGPS = Cast<AMGPlayerState>(PS))
		{
			SortedPlayers.Add(MGPS);
		}
	}

	SortedPlayers.Sort([](
		const AMGPlayerState& A,
		const AMGPlayerState& B)
		{
			return A.TotalScore > B.TotalScore;
		});


	for (int32 i = 0; i < SortedPlayers.Num(); i++)
	{
		SortedPlayers[i]->Rank = i + 1;
	}

	return SortedPlayers;
}