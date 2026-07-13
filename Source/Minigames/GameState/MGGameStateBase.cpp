// MGGameStateBase.cpp


#include "GameState/MGGameStateBase.h"

#include "Controller/MGPlayerController.h"
#include "Net/UnrealNetwork.h"

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
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (AMGPlayerController* MGPC = Cast<AMGPlayerController>(PC))
			{
				MGPC->HideMinigameIntro();
			}
		}
	}
}
