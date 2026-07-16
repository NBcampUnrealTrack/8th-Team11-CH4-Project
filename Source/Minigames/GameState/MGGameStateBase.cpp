// MGGameStateBase.cpp


#include "GameState/MGGameStateBase.h"
#include "GameState/MGPassBombGameState.h"
#include "GameState/MGFlagGameStateBase.h"

#include "Controller/MGPlayerController.h"
#include "Net/UnrealNetwork.h"

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

	return EMinigameType::NONE;
}

void AMGGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MatchState);
	DOREPLIFETIME(ThisClass, RoundState);
	DOREPLIFETIME(ThisClass, AliveCharacters);
}

void AMGGameStateBase::OnRep_MatchState()
{
	// 게임 시작(Waiting 종료)되면 인트로 숨김
	if (MatchState == EMatchState::PlayingCutScene || MatchState == EMatchState::Playing)
	{
		// 데디 서버엔 로컬 플레이어/뷰포트가 없어 nullptr이 반환되어 자연히 무시됨.
		// 리슨 서버(호스트)와 클라이언트는 각자의 로컬 PC를 대상으로 숨김.
		if (APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController())
		{
			if (AMGPlayerController* MGPC = Cast<AMGPlayerController>(PC))
			{
				MGPC->HideMinigameIntro();
			}
		}
	}
}
