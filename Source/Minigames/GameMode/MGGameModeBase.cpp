// MGGameModeBase.cpp


#include "GameMode/MGGameModeBase.h"

#include "Controller/MGPlayerController.h"
#include "GameState/MGGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/MGFlagPlayerState.h"

AMGGameModeBase::AMGGameModeBase()
{
	bUseSeamlessTravel = true;		// 심리스 트래블 기능 활성화
	// PlayerController : Interaction Key 유지 ( E, F, R, Q 등 )
	// PlayerState : 플레이어별 점수, 등수 등 기록용
	// 필요에 따라서는 PlayerCharacter도 유지
}

void AMGGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AMGGameStateBase* MGGameState = GetGameState<AMGGameStateBase>();
	if (IsValid(MGGameState) == false)
	{
		return;
	}

	if (MGGameState->MatchState != EMatchState::Waiting)
	{
		NewPlayer->SetLifeSpan(0.1f);
		return;
	}

	AMGPlayerController* NewPlayerController = Cast<AMGPlayerController>(NewPlayer);
	if (IsValid(NewPlayerController) == true)
	{
		AllPlayerControllers.Add(NewPlayerController);

		NewPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
	}
}

void AMGGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	AMGPlayerController* ExitingPlayerController = Cast<AMGPlayerController>(Exiting);
	if (IsValid(ExitingPlayerController) == true && AllPlayerControllers.Find(ExitingPlayerController) != INDEX_NONE)
	{
		AllPlayerControllers.Remove(ExitingPlayerController);
	}
}

void AMGGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.f, true);

	RemainWaitingTimeForPlaying = WaitingTime;

	RemainWaitingTimeForEnding = EndingTime;
}

void AMGGameModeBase::StartMinigame()
{
	AMGGameStateBase* MGGameState = GetGameState<AMGGameStateBase>();
	MGGameState->MatchState = EMatchState::Playing;
}

void AMGGameModeBase::EndMinigame()
{
	AMGGameStateBase* MGGameState = GetGameState<AMGGameStateBase>();
	MGGameState->MatchState = EMatchState::Ending;
}

void AMGGameModeBase::OnCharacterDead(AMGPlayerController* InController)
{
	if (IsValid(InController) == false || AllPlayerControllers.Find(InController) == INDEX_NONE)
	{
		return;
	}

	InController->ClientRPCShowGameResultWidget(AllPlayerControllers.Num());

	AllPlayerControllers.Remove(InController);
}

void AMGGameModeBase::GiveScore(AMGPlayerState* PS, int32 Rank)
{
	int32 PlayerCount = GameState->PlayerArray.Num();

	int32 AddScore = 0;
	if (Rank == 1)
	{
		AddScore = 10;
	}
	else
	{
		AddScore = FMath::FloorToInt(9.f / (PlayerCount - 1) * (PlayerCount - Rank)) + 1;
	}

	PS->SetScore(PS->GetScore() + AddScore);
}

void AMGGameModeBase::OnMainTimerElapsed()
{
	AMGGameStateBase* MGGameState = GetGameState<AMGGameStateBase>();
	if (IsValid(MGGameState) == false)
	{
		return;
	}

	switch (MGGameState->MatchState)
	{
	case EMatchState::None:
		break;
	case EMatchState::Waiting:
	{
		FString NotificationString = FString::Printf(TEXT(""));

		if (AllPlayerControllers.Num() < MinimumPlayerCountForPlaying)
		{
			NotificationString = FString::Printf(TEXT("Wait another players for playing."));

			RemainWaitingTimeForPlaying = WaitingTime; // 최소인원이 안된다면 대기 시간 초기화.
		}
		else
		{
			NotificationString = FString::Printf(TEXT("Wait %d seconds for playing."), RemainWaitingTimeForPlaying);

			--RemainWaitingTimeForPlaying;
		}

		if (RemainWaitingTimeForPlaying <= 0)
		{
			NotificationString = FString::Printf(TEXT(""));

			StartMinigame();
		}

		NotifyToAllPlayer(NotificationString);

		break;
	}
	case EMatchState::Ending:
	{
		FString NotificationString = FString::Printf(TEXT("Waiting %d for returning to title."), RemainWaitingTimeForEnding);

		NotifyToAllPlayer(NotificationString);

		--RemainWaitingTimeForEnding;

		if (RemainWaitingTimeForEnding <= 0)
		{
			for (auto MGPC : AllPlayerControllers)
			{
				MGPC->ClientRPCReturnToTitle();
			}

			MainTimerHandle.Invalidate();

			FName CurrentLevelName = FName(UGameplayStatics::GetCurrentLevelName(this));
			UGameplayStatics::OpenLevel(this, CurrentLevelName, true, FString(TEXT("listen")));

			return;
		}

		break;
	}
	default:
		break;
	}
}

void AMGGameModeBase::NotifyToAllPlayer(const FString& NotificationString)
{
	for (auto MGPC : AllPlayerControllers)
	{
		MGPC->NotificationText = FText::FromString(NotificationString);
	}
}