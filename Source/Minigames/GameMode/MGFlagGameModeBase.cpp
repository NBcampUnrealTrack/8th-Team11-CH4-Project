// MGFlagGameModeBase.cpp


#include "GameMode/MGFlagGameModeBase.h"
#include "GameState/MGFlagGameStateBase.h"
#include "PlayerState/MGFlagPlayerState.h"

void AMGFlagGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMGFlagGameModeBase::StartMiniGame()
{
	Super::StartMiniGame();
	
	GetWorld()->GetTimerManager().SetTimer(
		GameTimerHandle,
		this,
		&ThisClass::OnGameTimerElapsed,
		1.f,
		true
	);
	
	// TODO: 깃발 스폰
}

void AMGFlagGameModeBase::OnGameTimerElapsed()
{
	--RemainGameTime;
	if (AMGFlagGameStateBase* FGS = GetGameState<AMGFlagGameStateBase>())
	{
		FGS->RemainGameTime = RemainGameTime;
	}
	
	if (RemainGameTime == 0)
	{
		EndGame();
	}
}

void AMGFlagGameModeBase::EndGame()
{
	DeterMineWinner();
	GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);
	if (AMGFlagGameStateBase* FlagGameState = GetGameState<AMGFlagGameStateBase>())
	{
		// TODO: Round 수정
		FlagGameState->MatchState = EMatchState::Ending;
	}
}

void AMGFlagGameModeBase::DeterMineWinner()
{
	TArray<AMGFlagPlayerState*> FlagPlayerStates;

	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (AMGFlagPlayerState* FlagPS = Cast<AMGFlagPlayerState>(PS))
		{
			FlagPlayerStates.Add(FlagPS);
		}
	}
	
	FlagPlayerStates.Sort(
		[](const AMGFlagPlayerState& A, const AMGFlagPlayerState& B)
		{
			return A.HoldingTime > B.HoldingTime;
		}
	);
	
	int32 Rank = 1;
	for (int32 i = 0; i < FlagPlayerStates.Num(); i++)
	{
		if (i > 0 && FlagPlayerStates[i]->HoldingTime < FlagPlayerStates[i-1]->HoldingTime)
		{
			Rank = i + 1;
		}
		FlagPlayerStates[i]->Rank = Rank;
		GiveScore(FlagPlayerStates[i], Rank);
	}
}

void AMGFlagGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);
}