// MGGameModeBase.cpp


#include "GameMode/MGGameModeBase.h"

#include "Controller/MGPlayerController.h"
#include "GameState/MGGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/MGFlagPlayerState.h"
#include "GameInstance/MGGameInstance.h"				// GameInstance

#include "Minigames.h"				// 커스텀 Log

AMGGameModeBase::AMGGameModeBase()
{
	bUseSeamlessTravel = true;		// 심리스 트래블 기능 활성화
	// PlayerController : Interaction Key 유지 ( E, F, R, Q 등 )
	// PlayerState : 플레이어별 점수, 등수 등 기록용
	// 필요에 따라서는 PlayerCharacter도 유지
}

void AMGGameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	// GameInstance를 가져와서 현재 전체 세션의 라운드 상태를 확인
	UMGGameInstance* MGGameInstance = Cast<UMGGameInstance>(GetGameInstance());

	if (IsValid(MGGameInstance))
	{
		// 로비 상태가 아니라면 (Round1, Round2, Round3, FinalResult) 접속 차단
		if (MGGameInstance->CurrentRoundState != ERoundState::Lobby)
		{
			ErrorMessage = TEXT("The tournament has already started. You can only join in the Lobby.");
			// 에러 메세지에 유효한 값이 있으면 접속 차단
			return;
		}
	}

	// TODO : 만약 미니게임 도중에 접속을 허용하려면 로직을 변경해야함
	// ex) Round2에서 재접속 시, 현재 미니게임에선 관전자로 전환하고 Round3에선 정상 플레이 가능
}

void AMGGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AMGGameStateBase* MGGameState = GetGameState<AMGGameStateBase>();
	if (IsValid(MGGameState) == false)
	{
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

	// 30초 후 SeamlessTravel 하는지 테스트용
	FTimerHandle TestEndTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TestEndTimerHandle, this, &ThisClass::EndMinigame, 30.f, false);
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

	// AllPlayerControllers.Remove(InController);
}

void AMGGameModeBase::GiveScore(AMGPlayerState* PS, int32 Rank)
{
	/*
	int32 PlayerCount = GameState->PlayerArray.Num();

	int32 AddScore = 0;

	if (Rank == 1 || PlayerCount <= 1)
	{
		AddScore = 10;
	}
	else
	{
		AddScore = FMath::FloorToInt(9.f / (PlayerCount - 1) * (PlayerCount - Rank)) + 1;
	}

	PS->SetScore(PS->GetScore() + AddScore);
	*/


	// Seamless Travel Test Begin-------------------
	int32 RandomScore = FMath::RandRange(10, 100);

	// 커스텀 변수 MGScore 갱신
	PS->SetMGScore(PS->GetMGScore() + RandomScore);

	// 커스텀 로그 매크로를 사용하여 Travel 전 점수 확인
	MG_LOG_NET(LogMGNet, Warning, TEXT("[Before Travel] Player : %s | Added : %d | Total MGScore : %d"),
		*PS->GetPlayerName(), RandomScore, PS->GetMGScore());

	PS->Client_LogScoreBeforeTravel(RandomScore, PS->GetMGScore());
	// Seamless Travel Test End-------------------
}

void AMGGameModeBase::OnMainTimerElapsed()
{
	AMGGameStateBase* MGGameState = GetGameState<AMGGameStateBase>();
	if (IsValid(MGGameState) == false)
	{
		return;
	}

	// 임시 Tick 디버깅 메세지
	UE_LOG(LogTemp, Warning, TEXT("[State Check] Current Map: %s | MatchState: %d"), 
		*GetWorld()->GetMapName(), (int32)MGGameState->MatchState);

	// TODO : Lobby -> Minigame1, 2, 3, ..., -> FinalResult Level -> Lobby Level로
	// 모든 레벨에서 Seamless Travel을 사용할 경우
	// MGGameInstance->CurrentRoundState == ERoundState::Lobby인 경우
	// 최초로 Lobby에서 Minigame으로 넘어왔을때 초기화 작업이 필요할 수도 있음

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
		// 남은 시간 알림
		FString NotificationString = FString::Printf(TEXT("Moving to next stage in %d seconds..."), RemainWaitingTimeForEnding);
		NotifyToAllPlayer(NotificationString);


		// Seamless Travel Test Begin-------------------
		if (RemainWaitingTimeForEnding == 1)
		{
			if (IsValid(MGGameState))
			{
				UE_LOG(LogTemp, Warning, TEXT("=========== [Test] PlayerArray Num : %d ==========="), MGGameState->PlayerArray.Num());

				for (APlayerState* BasePS : MGGameState->PlayerArray)
				{
					if (AMGPlayerState* MGPS = Cast<AMGPlayerState>(BasePS))
					{
						GiveScore(MGPS, 0);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Cast to AMGPlayerState Failed!"));
					}
				}
			}
		}
		// Seamless Travel Test End-------------------

		--RemainWaitingTimeForEnding;

		// 카운트다운 종료 시 맵 이동
		if (RemainWaitingTimeForEnding <= 0)
		{
			MainTimerHandle.Invalidate();

			// 심리스 트래블을 하더라도 초기화 되지 않는 GameInstance에 저장되어있는 현재 라운드 정보를 가져옴
			UMGGameInstance* MGGameInstance = Cast<UMGGameInstance>(GetGameInstance());
			FString NextMapURL = TEXT("");

			if (IsValid(MGGameInstance))
			{
				// CurrentRoundState에 따라 다음 맵과 다음 라운드 상태를 갱신
				switch (MGGameInstance->CurrentRoundState)
				{
				case ERoundState::Round1:
				{
					NextMapURL = MGGameInstance->GetLevelURLForRoundState(ERoundState::Round2);
					MGGameInstance->CurrentRoundState = ERoundState::Round2;
					break;
				}
			
				case ERoundState::Round2:
				{
					NextMapURL = MGGameInstance->GetLevelURLForRoundState(ERoundState::Round3);
					MGGameInstance->CurrentRoundState = ERoundState::Round3;
					break;
				}
				case ERoundState::Round3:
				{
					// FinalResult을 별도 맵에 진행할거면 그곳으로, 아니라면 바로 로비로 이동(현재)
					NextMapURL = TEXT("/Game/Minigames/Level/L_Lobby");
					MGGameInstance->CurrentRoundState = ERoundState::FinalResult;
					break;
				}
				case ERoundState::FinalResult:
				{
					// 결과창에서 로비로 완전히 돌아가는 처리
					NextMapURL = TEXT("/Game/Minigames/Level/L_Lobby");
					MGGameInstance->CurrentRoundState = ERoundState::Lobby;
					break;
				}
				default:
					break;
				}

				// Return To Lobby Debug Begin -----------------
				UE_LOG(LogTemp, Warning, TEXT("[Travel Check] CurrentRoundState: %d | NextMapURL: %s"),
					(int32)MGGameInstance->CurrentRoundState, *NextMapURL);

				// 심리스 트래블 실행 (클라이언트들은 자동으로 서버를 따라옴)
				if (NextMapURL.IsEmpty() == false)
				{
					UE_LOG(LogTemp, Warning, TEXT("[Travel Check] Executing ServerTravel..."));
					GetWorld()->ServerTravel(NextMapURL);
				}
				else
				{
					// NextMapURL이 비어있어서 트래블이 취소되었을 때 에러 로그
					UE_LOG(LogTemp, Error, TEXT("[Travel Check] NextMapURL is EMPTY! ServerTravel Canceled."));
				}

				return;
				// Return To Lobby Debug End -----------------
			}

			// 심리스 트래블 실행 (클라이언트들은 자동으로 서버를 따라옴)
			if (NextMapURL.IsEmpty() == false)
			{
				GetWorld()->ServerTravel(NextMapURL);
			}

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