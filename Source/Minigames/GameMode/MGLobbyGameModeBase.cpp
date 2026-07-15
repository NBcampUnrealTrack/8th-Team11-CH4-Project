// MGLobbyGameModeBase.cpp


#include "GameMode/MGLobbyGameModeBase.h"

#include "Controller/MGPlayerController.h"
#include "PlayerState/MGLobbyPlayerState.h"
#include "GameState/MGLobbyGameStateBase.h"
#include "GameInstance/MGGameInstance.h"

AMGLobbyGameModeBase::AMGLobbyGameModeBase()
{
	bUseSeamlessTravel = true;
	
	GameStateClass   = AMGLobbyGameStateBase::StaticClass();
	PlayerStateClass = AMGLobbyPlayerState::StaticClass();
	
	AvailableColors = {
		EMGPlayerColor::Red, EMGPlayerColor::Orange, EMGPlayerColor::Yellow,
		EMGPlayerColor::Green, EMGPlayerColor::Blue, EMGPlayerColor::Indigo,
		EMGPlayerColor::Purple, EMGPlayerColor::Pink, EMGPlayerColor::White,
		EMGPlayerColor::Gray
	};
}

void AMGLobbyGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

	Super::EndPlay(EndPlayReason);
}

void AMGLobbyGameModeBase::PreLogin(const FString& Options, const FString& Address,
	const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	
	if (AllPlayerControllers.Num() >= MaxPlayerCount)
	{
		ErrorMessage = TEXT("Lobby is full");
		// TODO: 정원 초과 안내 UI. ErrorString을 위젯에 표시 (3~4주차 UI 작업 때 같이 처리)
	}
}

void AMGLobbyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	AMGPlayerController* PC = Cast<AMGPlayerController>(NewPlayer);
	if (IsValid(PC))
	{
		AllPlayerControllers.Add(PC);
		
		AssignRandomColorToPlayer(PC->GetPlayerState<AMGLobbyPlayerState>());
		
		if (UMGGameInstance* GI = GetGameInstance<UMGGameInstance>())
		{
			if (IsValid(PC->PlayerState))
			{
				const FUniqueNetIdRepl Id = PC->PlayerState->GetUniqueId();
				if (Id.IsValid())
				{
					GI->LobbyPlayerIds.Add(Id.GetUniqueNetId()->ToString());
				}
			}
		}
		
		if (AMGLobbyGameStateBase* LGS = GetGameState<AMGLobbyGameStateBase>())
		{
			LGS->SetCurrentPlayerCount(AllPlayerControllers.Num());
		}
		
		// TODO: 레디 버튼 기본값은 비활성화.
		//       로그인 후 인원이 MinimumPlayerCount 이상이면 전원 레디 버튼 활성화 (Client RPC)
	}
}

void AMGLobbyGameModeBase::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C); // 부모 함수 먼저 호출 (여기서 PlayerState 등이 세팅됨)

	AMGPlayerController* PC = Cast<AMGPlayerController>(C);
	if (IsValid(PC))
	{
		AllPlayerControllers.AddUnique(PC);

		if (AMGLobbyGameStateBase* LGS = GetGameState<AMGLobbyGameStateBase>())
		{
			LGS->SetCurrentPlayerCount(AllPlayerControllers.Num());
		}

		// 로비로 돌아와도 PlayerColor 유지
		AssignRandomColorToPlayer(PC->GetPlayerState<AMGLobbyPlayerState>());
		PC->ClientRPCOnSeamlessTravelCompleted();

		// TODO: 레디 버튼 기본값은 비활성화.
		//       로그인 후 인원이 MinimumPlayerCount 이상이면 전원 레디 버튼 활성화 (Client RPC)
	}
}

void AMGLobbyGameModeBase::Logout(AController* ExitingController)
{
	Super::Logout(ExitingController);

	AMGPlayerController* PC = Cast<AMGPlayerController>(ExitingController);
	if (IsValid(PC) == false || AllPlayerControllers.Find(PC) == INDEX_NONE)
	{
		return;
	}

	AllPlayerControllers.Remove(PC);
	
	if (AMGLobbyGameStateBase* LGS = GetGameState<AMGLobbyGameStateBase>())
	{
		LGS->SetCurrentPlayerCount(AllPlayerControllers.Num());
	}
			
	AMGLobbyPlayerState* PS = PC->GetPlayerState<AMGLobbyPlayerState>();
	if (IsValid(PS))
	{
		AvailableColors.Add(PS->PlayerColor);
	}
	
	// TODO: 로그아웃 후 남은 인원이 MinimumPlayerCount 미만이면 전원 레디 버튼 비활성화 (Client RPC)
	
	// 카운트다운 진행 중 로그아웃 된 플레이어가 있다면 전원 레디 해제
	if (GetWorldTimerManager().IsTimerActive(CountdownTimerHandle))
	{
		CancelCountdown();

		for (AMGPlayerController* Remaining : AllPlayerControllers)
		{
			AMGLobbyPlayerState* RemainingPS = Remaining->GetPlayerState<AMGLobbyPlayerState>();
			if (IsValid(RemainingPS))
			{
				RemainingPS->SetReady(false);
			}
		}
	}
}

void AMGLobbyGameModeBase::OnPlayerReady(AMGPlayerController* PC, bool bReady)
{
	AMGLobbyPlayerState* PS = PC->GetPlayerState<AMGLobbyPlayerState>();
	if (IsValid(PS) == false)
	{
		return;
	}

	PS->SetReady(bReady);
	if (bReady)
	{
		CheckAndStartCountdown();
	}
	else
	{
		CancelCountdown();
	}
}

void AMGLobbyGameModeBase::CheckAndStartCountdown()
{
	if (AllPlayerControllers.Num() < MinimumPlayerCount)
	{
		return;
	}

	if (CheckAllPlayersReady() == false)
	{
		return;
	}

	if (GetWorldTimerManager().IsTimerActive(CountdownTimerHandle))
	{
		return;
	}

	AMGLobbyGameStateBase* LGS = GetGameState<AMGLobbyGameStateBase>();
	if (IsValid(LGS))
	{
		LGS->SetRemainCountdownTime(CountdownTime);
	}
	
	GenerateMinigameSequence();
	
	GetWorldTimerManager().SetTimer(
	   CountdownTimerHandle,
	   this,
	   &ThisClass::OnCountdownElapsed,
	   1.f,
	   true
   );
}

void AMGLobbyGameModeBase::OnCountdownElapsed()
{
	AMGLobbyGameStateBase* LGS = GetGameState<AMGLobbyGameStateBase>();
	if (IsValid(LGS) == false)
	{
		return;
	}

	const int32 NewTime = LGS->GetRemainCountdownTime() - 1;
	LGS->SetRemainCountdownTime(NewTime);
	
	if (NewTime > 0)
	{
		return;
	}

	CancelCountdown();
	// TODO: 모든 플레이어 알림
	TravelToMinigameLevel();
}

void AMGLobbyGameModeBase::CancelCountdown()
{
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	
	AMGLobbyGameStateBase* LGS = GetGameState<AMGLobbyGameStateBase>();
	if (IsValid(LGS))
	{
		LGS->SetRemainCountdownTime(0);
	}
}

void AMGLobbyGameModeBase::TravelToMinigameLevel()
{
	UMGGameInstance* GI = GetGameInstance<UMGGameInstance>();
	if (IsValid(GI) == false)
	{
		return;
	}

	const FString URL = GI->GetLevelURLForRound(0);   // 첫 라운드
	if (URL.IsEmpty())
	{
		return;
	}

	GI->CurrentRoundState = ERoundState::Round1;
	GetWorld()->ServerTravel(URL); 
}

void AMGLobbyGameModeBase::GenerateMinigameSequence()
{
	UMGGameInstance* GI = GetGameInstance<UMGGameInstance>();
	if (IsValid(GI) == false)
	{
		return;
	}
	
	TArray<EMinigameType> Pool;
	GI->MinigameLevels.GenerateKeyArray(Pool);
	
	for (int32 i = Pool.Num() - 1; i > 0; --i)
	{
		const int32 j = FMath::RandRange(0, i);
		Pool.Swap(i, j);
	}
	
	const int32 Count = FMath::Min(TotalRoundCount, Pool.Num());
	GI->MinigameSequence.Reset();
	for (int32 i = 0; i < Count; ++i)
	{
		GI->MinigameSequence.Add(Pool[i]);
	}
}

void AMGLobbyGameModeBase::OnPlayerChangeColor(AMGPlayerController* PC, EMGPlayerColor NewColor)
{
	AMGLobbyPlayerState* PS = PC->GetPlayerState<AMGLobbyPlayerState>();
	if (IsValid(PS) == false)
	{
		return;
	}

	const int32 Index = AvailableColors.Find(NewColor);
	if (Index == INDEX_NONE)
	{
		return;   // 이미 다른 사람이 쓰는 중
	}

	AvailableColors.RemoveAtSwap(Index);
	AvailableColors.Add(PS->PlayerColor);   // 원래 쓰던 색은 반납
	PS->SetPlayerColor(NewColor);
	
	if (UMGGameInstance* GI = GetGameInstance<UMGGameInstance>())
	{
		GI->SavedPlayerData.FindOrAdd(PS->GetUniqueId()).Color = PS->PlayerColor;
	}
}

void AMGLobbyGameModeBase::AssignRandomColorToPlayer(AMGLobbyPlayerState* PS)
{
	if (IsValid(PS) == false)
	{
		return;
	}
	
	UMGGameInstance* GI = GetGameInstance<UMGGameInstance>();
	// 저장된 색이 있으면 복원 (재접속/복귀 대비)
	if (IsValid(GI))
	{
		if (const FMGPlayerSaveData* Saved = GI->SavedPlayerData.Find(PS->GetUniqueId()))
		{
			PS->SetPlayerColor(Saved->Color);
			AvailableColors.Remove(Saved->Color);   // 풀에서 제거해 중복 방지
			return;
		}
	}
	
	// 없으면 새로 랜덤 배정
	if (AvailableColors.Num() == 0)
	{
		return;
	}
	
	const int32 RandomIndex = FMath::RandRange(0, AvailableColors.Num() - 1);
	PS->SetPlayerColor(AvailableColors[RandomIndex]);
	AvailableColors.RemoveAtSwap(RandomIndex);
	
	if (IsValid(GI))
	{
		GI->SavedPlayerData.FindOrAdd(PS->GetUniqueId()).Color = PS->PlayerColor;
	}
}

bool AMGLobbyGameModeBase::CheckAllPlayersReady()
{
	for (AMGPlayerController* PC : AllPlayerControllers)
	{
		AMGLobbyPlayerState* PS = PC->GetPlayerState<AMGLobbyPlayerState>();
		if (IsValid(PS) == false || PS->bIsReady == false)
		{
			return false;
		}
	}
	return true;
}
