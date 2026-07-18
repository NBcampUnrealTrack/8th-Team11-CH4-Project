// MGGameModeBase.cpp


#include "GameMode/MGGameModeBase.h"

#include "Controller/MGPlayerController.h"
#include "GameState/MGGameStateBase.h"
#include "PlayerState/MGFlagPlayerState.h"
#include "GameInstance/MGGameInstance.h"

#include "MGNetConfig.h"
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
			// 로비를 거친 플레이어면 재접속 허용 (심리스 실패 대비)
			const bool bIsKnownPlayer = UniqueId.IsValid()
				&& MGGameInstance->LobbyPlayerIds.Contains(UniqueId.GetUniqueNetId()->ToString());
			if (bIsKnownPlayer == false)
			{
				// 모르는 외부인만 차단
				ErrorMessage = TEXT("The tournament has already started. You can only join in the Lobby.");
			}
			return;
		}
	}

	// TODO : 만약 미니게임 도중에 접속을 허용하려면 로직을 변경해야함
	// ex) Round2에서 재접속 시, 현재 미니게임에선 관전자로 전환하고 Round3에선 정상 플레이 가능
}

void AMGGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	MG_LOG_NET(LogMGNet, Log, TEXT("%s has Login."), *NewPlayer->GetName());

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

		if (UMGGameInstance* GI = GetGameInstance<UMGGameInstance>())
		{
			GI->RestorePlayerData(NewPlayerController->GetPlayerState<AMGPlayerState>());
		}
	}
}


void AMGGameModeBase::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);
    
	AMGGameStateBase* MGGameState = GetGameState<AMGGameStateBase>();
	if (IsValid(MGGameState) == false)
	{
		return;
	}

	AMGPlayerController* NewPlayerController = Cast<AMGPlayerController>(C);
	if (IsValid(NewPlayerController) == true)
	{
		AllPlayerControllers.AddUnique(NewPlayerController);

		NewPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		if (UMGGameInstance* GI = GetGameInstance<UMGGameInstance>())
		{
			GI->RestorePlayerData(NewPlayerController->GetPlayerState<AMGPlayerState>());
		}
		NewPlayerController->ClientRPCOnSeamlessTravelCompleted();
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
	
	bUseSeamlessTravel = true; 

	GetWorld()->GetTimerManager().SetTimer(
		MainTimerHandle, 
		this, 
		&ThisClass::OnMainTimerElapsed, 
		1.f, 
		true
	);

	RemainWaitingTimeForPlaying = WaitingTime;
	RemainWaitingTimeForEnding = EndingTime;
	RemainEnteringWaitTime = EnteringMaxWaitTime;
}

#pragma region CutScene

void AMGGameModeBase::PlayCutScene()
{
	AMGGameStateBase* MGGameState = GetGameState<AMGGameStateBase>();
	if (IsValid(MGGameState))
	{
		MGGameState->SetMatchState(EMatchState::PlayingCutScene);
	}

	for (TObjectPtr<AMGPlayerController> PC : AllPlayerControllers)
	{
		// 배열에 들어있더라도 그 사이 플레이어가 접속을 끊었을 수도 있으니 항상 IsValid 체크
		if (!IsValid(PC))
		{
			bUseCutScene = false;
			break;
		}

		const TArray<TObjectPtr<ULevelSequence>>& Assets = PC->GetCutSceneAssets();

		// 컷씬 에셋이 없거나 0번 인덱스가 유효하지 않으면 컷씬 사용 안 함
		if (Assets.Num() == 0 || Assets[0] == nullptr)
		{
			bUseCutScene = false;
			break;
		}
	}

	if (bUseCutScene)
	{
		for (TObjectPtr<AMGPlayerController> PC : AllPlayerControllers)
		{
			if (IsValid(PC))
			{
				// TODO : 현재는 0번 Index의 Level Sequence 사용
				// 추후 Level Sequence가 추가된다면 Game Instance에서 Index 관리 필요
				PC->ClientRPC_PlayCutScene(0);
			}
		}

		// TODO : 현재 CutsceneDuration 하드코딩
		// 추후 Level Sequence 추가 시 Game Instance에서 관리 필요
		// 실제 Level Sequence 길이보다 1~2초 길게 설정 필요
		const float CutsceneDuration = 23.f;

		GetWorldTimerManager().SetTimer(
			CutSceneTimerHandler,
			this,
			&AMGGameModeBase::OnFinishedCutScene,
			CutsceneDuration,
			false
		);
	}
	else
	{
		OnFinishedCutScene();
	}
}

void AMGGameModeBase::OnFinishedCutScene()
{
	StartMinigame();
}

#pragma endregion

void AMGGameModeBase::StartMinigame()
{
	AMGGameStateBase* MGGameState = GetGameState<AMGGameStateBase>();
	if (IsValid(MGGameState))
	{
		MGGameState->SetMatchState(EMatchState::Playing);
	}
}

void AMGGameModeBase::EndMinigame()
{
	AMGGameStateBase* MGGameState = GetGameState<AMGGameStateBase>();
	MGGameState->SetMatchState(EMatchState::Ending);
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

	const int32 PrevMGScore = PS->GetMGScore();
	const int32 PrevTotalScore = PS->TotalScore;
	
	PS->SetMGScore(PS->GetMGScore() + AddScore);
	PS->TotalScore += AddScore;
	PS->RoundScores.Add(AddScore);
	
	UE_LOG(LogMGNet, Log, TEXT("[GiveScore] %s | Rank %d/%d | +%d점 | MGScore %d->%d | TotalScore %d->%d"),
		*PS->GetPlayerName(), Rank, PlayerCount, AddScore,
		PrevMGScore, PS->GetMGScore(), PrevTotalScore, PS->TotalScore);
}

void AMGGameModeBase::OnMainTimerElapsed()
{
	AMGGameStateBase* MGGameState = GetGameState<AMGGameStateBase>();
	if (ensure(IsValid(MGGameState)) == false)
	{
		return;
	}

	// Test Log
	UE_LOG(LogMGNet, Verbose, TEXT("[State Check] Current Map: %s | MatchState: %d"), 
		*GetWorld()->GetMapName(), (int32)MGGameState->GetMatchState());

	// TODO : Lobby -> Minigame1, 2, 3, ..., -> FinalResult Level -> Lobby Level로
	// 모든 레벨에서 Seamless Travel을 사용할 경우
	// MGGameInstance->CurrentRoundState == ERoundState::Lobby인 경우
	// 최초로 Lobby에서 Minigame으로 넘어왔을때 초기화 작업이 필요할 수도 있음

	switch (MGGameState->GetMatchState())
	{
	case EMatchState::None:
		{
			break;
		}
	case EMatchState::Entering:
		{
			UMGGameInstance* GI = Cast<UMGGameInstance>(GetGameInstance());
			const int32 Expected = IsValid(GI) ? GI->TournamentPlayerCount : 0;

			const bool bEveryoneArrived = (Expected > 0 && AllPlayerControllers.Num() >= Expected);
			--RemainEnteringWaitTime;
			const bool bTimedOut = (RemainEnteringWaitTime <= 0);

			const bool bForDebug = MG_USE_EOS == 0;	// MG_USE_EOS == 0이면 디버깅으로 인식하여 인원 수 상관없이 시작이 가능.

			if (bEveryoneArrived || bTimedOut || bForDebug)
			{
				RemainWaitingTimeForPlaying = WaitingTime;   // 전원 도착 시점부터 카운트다운
				MGGameState->SetMatchState(EMatchState::Waiting);
			}
			else
			{
				NotifyToAllPlayer(FString::Printf(TEXT("Waiting for players... (%d/%d)"),
					AllPlayerControllers.Num(), Expected));
			}
			break;
		}
	case EMatchState::Waiting:
		{
			// Test Log
			UE_LOG(LogTemp, Verbose, TEXT("[State Check] Current Map: %s | MatchState: %d | Players: %d | RemainWait: %d"),
			 	*GetWorld()->GetMapName(), (int32)MGGameState->GetMatchState(), AllPlayerControllers.Num(), RemainWaitingTimeForPlaying);

			FString NotificationString = FString::Printf(TEXT("Round starts in %d seconds..."), RemainWaitingTimeForPlaying);
	
			if (RemainWaitingTimeForPlaying <= 0)
			{
				NotificationString = FString::Printf(TEXT(""));
	
				PlayCutScene();
				MGGameState->OnRep_MatchState();
			}
	
			NotifyToAllPlayer(NotificationString);

			--RemainWaitingTimeForPlaying;
			break;
		}
	case EMatchState::Ending:
		{
			// 남은 시간 알림
			FString NotificationString = FString::Printf(TEXT("Moving to next stage in %d seconds..."), RemainWaitingTimeForEnding);
			NotifyToAllPlayer(NotificationString);
	
			--RemainWaitingTimeForEnding;
	
			// 카운트다운 종료 시 맵 이동
			if (RemainWaitingTimeForEnding <= 0)
			{
				GetWorld()->GetTimerManager().ClearTimer(MainTimerHandle);
	
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
						NextMapURL = TEXT("/Game/Minigames/Level/L_FinalResult");
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
						{
							break;
						}
					}
	
					// Return To Lobby Debug Begin -----------------
					UE_LOG(LogTemp, Warning, TEXT("[Travel Check] CurrentRoundState: %d | NextMapURL: %s"),
						(int32)MGGameInstance->CurrentRoundState, *NextMapURL);
	
					// 심리스 트래블 실행 (클라이언트들은 자동으로 서버를 따라옴)
					if (NextMapURL.IsEmpty() == false)
					{
						for (APlayerState* PS : MGGameState->PlayerArray)
						{
							MGGameInstance->SavePlayerData(PS);
						}
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
		{
			break;
		}
	}
}

void AMGGameModeBase::NotifyToAllPlayer(const FString& NotificationString)
{
	for (auto PC : AllPlayerControllers)
	{
		PC->NotificationText = FText::FromString(NotificationString);
	}
}

// EndPlay에서 사용중인 모든 타이머를 안전하게 제거
void AMGGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(MainTimerHandle);

	// 상속받은 EndPlay의 Super는 마지막에
	Super::EndPlay(EndPlayReason);
}