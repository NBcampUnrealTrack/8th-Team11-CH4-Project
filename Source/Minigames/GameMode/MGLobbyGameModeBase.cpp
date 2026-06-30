// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/MGLobbyGameModeBase.h"

#include "Controller/MGPlayerController.h"
#include "PlayerState/MGLobbyPlayerState.h"
#include "GameState/MGLobbyGameStateBase.h"

AMGLobbyGameModeBase::AMGLobbyGameModeBase()
{
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
	// TODO: 미니게임 선택 로직
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
	// TODO: 미니게임 선택 결과에 따라 MinigameLevelName 결정 (현재는 에디터 고정값에 의존)
	// TODO: EMinigameType 기반 레벨 매핑 (TMap<EMinigameType, FString>) 적용 예정
	
	if (MinigameLevelName.IsEmpty())
	{
		return;
	}

	GetWorld()->ServerTravel(MinigameLevelName);
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
