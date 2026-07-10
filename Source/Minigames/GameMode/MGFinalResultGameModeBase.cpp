// MGFinalResultGameModeBase.cpp


#include "GameMode/MGFinalResultGameModeBase.h"

#include "Controller/MGPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "GameState/MGGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/MGPlayerState.h"

void AMGFinalResultGameModeBase::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);
	
	if (AMGPlayerController* PC = Cast<AMGPlayerController>(C))
	{
		PC->ClientRPC_SetResultCamera();
	}
}

void AMGFinalResultGameModeBase::StartMinigame()
{
	Super::StartMinigame();

	TArray<AMGPlayerState*> SortedPlayers = GetPlayersSortedByScore();
	PlacePlayersByRank(SortedPlayers);
	
	GetWorldTimerManager().SetTimer(
		AutoReturnTimerHandle, 
		this, 
		&ThisClass::ReturnToLobby, 
		20.f, 
		false
	);
	
	for (AMGPlayerController* PC : AllPlayerControllers)
	{
		if (IsValid(PC))
		{
			PC->ClientRPC_SetResultCamera();
			PC->ClientRPC_ShowFinalResult();
		}
	}
}

void AMGFinalResultGameModeBase::OnPlayerReadyToReturn(AMGPlayerController* PC)
{
	if (IsValid(PC) == false)
	{
		return;
	}
	if (AMGPlayerState* PS = PC->GetPlayerState<AMGPlayerState>())
	{
		PS->bReadyToReturn = true;
	}
	CheckAllReadyToReturn();
}

void AMGFinalResultGameModeBase::CheckAllReadyToReturn()
{
	// 이미 종료 진행 중이면 무시
	if (AMGGameStateBase* GS = GetGameState<AMGGameStateBase>())
	{
		if (GS->MatchState == EMatchState::Ending)
		{
			return;
		}
	}

	if (AllPlayerControllers.Num() == 0)
	{
		return;
	}

	// 접속 중인 전원이 눌렀는지 (AllPlayerControllers 기준 → 이탈자 자동 제외)
	for (AMGPlayerController* PC : AllPlayerControllers)
	{
		if (IsValid(PC) == false)
		{
			continue;
		}
		AMGPlayerState* PS = PC->GetPlayerState<AMGPlayerState>();
		if (IsValid(PS) == false || PS->bReadyToReturn == false)
		{
			return;   // 한 명이라도 안 눌렀으면 대기
		}
	}
	
	ReturnToLobby();
}

void AMGFinalResultGameModeBase::ReturnToLobby()
{
	GetWorldTimerManager().ClearTimer(AutoReturnTimerHandle);
	EndMinigame();
}

void AMGFinalResultGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	CheckAllReadyToReturn();
}

void AMGFinalResultGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(AutoReturnTimerHandle);
	
	Super::EndPlay(EndPlayReason);
}

TArray<AMGPlayerState*> AMGFinalResultGameModeBase::GetPlayersSortedByScore() const
{
	TArray<AMGPlayerState*> Players;
	if (IsValid(GameState))
	{
		for (APlayerState* PS : GameState->PlayerArray)
		{
			if (AMGPlayerState* MGPS = Cast<AMGPlayerState>(PS))
			{
				Players.Add(MGPS);
			}
		}
	}

	Players.Sort([](const AMGPlayerState& A, const AMGPlayerState& B)
	{
		return A.TotalScore > B.TotalScore;
	});

	return Players;
}

void AMGFinalResultGameModeBase::PlacePlayersByRank(const TArray<AMGPlayerState*>& SortedPlayers)
{
	AActor* PodiumMarkers[3];
	PodiumMarkers[0] = GetSingleActorByTag(TEXT("Podium_1"));
	PodiumMarkers[1] = GetSingleActorByTag(TEXT("Podium_2"));
	PodiumMarkers[2] = GetSingleActorByTag(TEXT("Podium_3"));

	TArray<AActor*> SpectatorMarkers;
	UGameplayStatics::GetAllActorsWithTag(this, TEXT("Spectator"), SpectatorMarkers);

	int32 SpectatorIndex = 0;
	for (int32 i = 0; i < SortedPlayers.Num(); i++)
	{
		SortedPlayers[i]->Rank = i + 1;

		APawn* Pawn = SortedPlayers[i]->GetPawn();
		if (IsValid(Pawn) == false)
		{
			continue;
		}

		AActor* Marker = nullptr;
		if (i < 3)
		{
			Marker = PodiumMarkers[i];
		}
		else if (SpectatorMarkers.IsValidIndex(SpectatorIndex))
		{
			Marker = SpectatorMarkers[SpectatorIndex];
			SpectatorIndex++;
		}

		if (IsValid(Marker))
		{
			Pawn->SetActorLocationAndRotation(Marker->GetActorLocation(), Marker->GetActorRotation());
		}
	}
}

AActor* AMGFinalResultGameModeBase::GetSingleActorByTag(FName Tag) const
{
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsWithTag((this), Tag, Found);
	return Found.Num() > 0 ? Found[0] : nullptr;
}


