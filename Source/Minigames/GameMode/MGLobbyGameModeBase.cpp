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
}

void AMGLobbyGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

	Super::EndPlay(EndPlayReason);
}

void AMGLobbyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	AMGPlayerController* PC = Cast<AMGPlayerController>(NewPlayer);
	if (IsValid(PC))
	{
		AllPlayerControllers.Add(PC);
		
		if (AMGLobbyGameStateBase* GS = GetGameState<AMGLobbyGameStateBase>())
		{
			GS->CurrentPlayerCount = AllPlayerControllers.Num();
		}
		
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
	
	if (AMGLobbyGameStateBase* GS = GetGameState<AMGLobbyGameStateBase>())
	{
		GS->CurrentPlayerCount = AllPlayerControllers.Num();
	}
	
	// TODO: 로그아웃 후 남은 인원이 MinimumPlayerCount 미만이면 전원 레디 버튼 비활성화 (Client RPC)

	// 카운트다운 진행 중이 아니면 레디 해제 불필요
	if (GetWorldTimerManager().IsTimerActive(CountdownTimerHandle) == false)
	{
		return;
	}

	CancelCountdown();

	for (AMGPlayerController* Remaining : AllPlayerControllers)
	{
		AMGLobbyPlayerState* PS = Remaining->GetPlayerState<AMGLobbyPlayerState>();
		if (IsValid(PS))
		{
			PS->SetReady(false);
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

void AMGLobbyGameModeBase::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C); // 부모 함수 먼저 호출 (여기서 PlayerState 등이 세팅됨)

	AMGPlayerController* PC = Cast<AMGPlayerController>(C);
	if (IsValid(PC))
	{
		AllPlayerControllers.AddUnique(PC);

		if (AMGLobbyGameStateBase* GS = GetGameState<AMGLobbyGameStateBase>())
		{
			GS->CurrentPlayerCount = AllPlayerControllers.Num();
		}

		// TODO: 레디 버튼 기본값은 비활성화.
		//       로그인 후 인원이 MinimumPlayerCount 이상이면 전원 레디 버튼 활성화 (Client RPC)
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

	AMGLobbyGameStateBase* GS = GetGameState<AMGLobbyGameStateBase>();
	if (IsValid(GS))
	{
		GS->RemainCountdownTime = CountdownTime;
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
	AMGLobbyGameStateBase* GS = GetGameState<AMGLobbyGameStateBase>();
	if (IsValid(GS) == false)
	{
		return;
	}

	--GS->RemainCountdownTime;

	if (GS->RemainCountdownTime > 0)
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
	
	AMGLobbyGameStateBase* GS = GetGameState<AMGLobbyGameStateBase>();
	if (IsValid(GS))
	{
		GS->RemainCountdownTime = 0;
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

	GI->CurrentRoundState = ERoundState::Round1;   // 로비 → 첫 라운드 (난입차단 정상화)
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
