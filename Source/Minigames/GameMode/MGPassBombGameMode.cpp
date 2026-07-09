// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/MGPassBombGameMode.h"
#include "Controller/MGPlayerController.h"
#include "Character/MGPlayerCharacter.h"
#include "PlayerState/MGPassBombPlayerState.h"
#include "GameState/MGPassBombGameState.h"
#include "Gimmick/MGBombActor.h"

#include "Minigames.h"

void AMGPassBombGameMode::StartMinigame()
{
	Super::StartMinigame();

	ExplodeTime = 5.f;

	AMGPassBombGameState* MGGS = GetGameState<AMGPassBombGameState>();
	checkf(IsValid(MGGS), TEXT("GameState is Invalid."));
	AlivePlayers = AllPlayerControllers;

	MGGS->AliveCharacters.Empty();
	for (int32 i = 0; i < AlivePlayers.Num(); i++)
	{
		MGGS->AliveCharacters.Add(AlivePlayers[i]->GetCharacter());
	}

	for (TObjectPtr<AMGPlayerController> PC : AlivePlayers)
	{
		// 배열에 들어있더라도 그 사이 플레이어가 접속을 끊었을 수도 있으니 항상 IsValid 체크
		if (IsValid(PC))
		{
			// TODO : 현재 0번 index를 하드코딩으로 사용하고 있으나
			// 추후 Level Sequence가 추가된다면 Game Instance에서 Index를 관리하도록 변경
			PC->ClientRPC_PlayCutScene(0);
		}
	}
	
	MG_LOG_NET(LogMGNet, Log, TEXT("AlivePlayer_Count: %d / AliveCharacter: %d"), AlivePlayers.Num(), MGGS->AliveCharacters.Num());


	// TODO : 현재 CutsceneDuration를 마찬가지로 하드코딩으로 사용하고 있으나
	// 추후 Level Sequence가 추가된다면 Game Instance에서 CutsceneDuration를 관리하도록 변경
	float CutsceneDuration = 19.f;
	GetWorldTimerManager().SetTimer(
		CutSceneTimerHandler,
		this,
		&AMGPassBombGameMode::OnFinishedCutScene,
		CutsceneDuration,
		false
	);
}

void AMGPassBombGameMode::OnFinishedCutScene()
{
	NextRound();
}

void AMGPassBombGameMode::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	BombActor = GetWorld()->SpawnActor<AMGBombActor>(BombActorClass,FTransform::Identity,SpawnParam);
}

void AMGPassBombGameMode::EliminatePlayer(ACharacter* TargetPlayer)
{
	AMGPassBombGameState* MGGS = GetGameState<AMGPassBombGameState>();
	AController* PC = TargetPlayer->GetController();

	if (IsValid(PC))
	{
		AMGPlayerController* MGPC = Cast<AMGPlayerController>(PC);
		if (IsValid(MGPC))
		{
			if (AlivePlayers.Contains(MGPC))
			{
				AMGPassBombPlayerState* MGPS = MGPC->GetPlayerState<AMGPassBombPlayerState>();
				if (IsValid(MGPS))
				{
					MGPS->MulticastRPC_RetireCharacter();
				}

				//TODO: 중간순위 점수 추가
				//MGPC->AddScore()

				AlivePlayers.Remove(MGPC);
				MGGS->AliveCharacters.Remove(MGPC->GetCharacter());
			}
			
			// 플레이어명 임시 지정, 이후 변경필요
			FString UserName;
			UserName = MGPC->GetPlayerState<AMGPassBombPlayerState>()->GetPlayerName();

			NotifyToAllPlayer(FString::Printf(TEXT("%s(이)가 탈락했습니다!"), *UserName));
			MG_LOG_NET(LogMGNet, Log, TEXT("AlivePlayer_Count: %d / AliveCharacter: %d"), AlivePlayers.Num(), MGGS->AliveCharacters.Num());
			GetWorldTimerManager().SetTimer(
				RoundTimerHandle,
				this,
				&ThisClass::NextRound,
				5.f,
				false
			);
		}
	}
}

void AMGPassBombGameMode::EndMinigame()
{
	Super::EndMinigame();
}

void AMGPassBombGameMode::NextRound()
{
	AMGPassBombGameState* MGGS = GetGameState<AMGPassBombGameState>();

	// 최후의 1인이 남을 때까지 반복
	if (AlivePlayers.Num() <= 1)
	{
		// 플레이어명 임시 지정, 이후 변경필요
		FString UserName;
		UserName = AlivePlayers[0]->GetPlayerState<AMGPassBombPlayerState>()->GetPlayerName();

		NotifyToAllPlayer(FString::Printf(TEXT("%s 승리!"), *UserName));

		// TODO: 승리자 점수 추가
		//AlivePlayers[0]->AddScore()

		// 게임 종료
		EndMinigame();
		return;
	}
	NotifyToAllPlayer(TEXT(""));

	// 술래 후보자
	TArray<AMGPlayerController*> BombNominee;

	// 점수가 가장 높은 사람들을 후보로 지명
	int32 MaxScore = -1;
	for (const auto MGPC : AlivePlayers)
	{
		const AMGPlayerState* MGPS = MGPC->GetPlayerState<AMGPlayerState>();
		if (IsValid(MGPS) == true)
		{
			if (MGPS->TotalScore > MaxScore)
			{
				BombNominee.Empty();
				BombNominee.Add(MGPC);
				MaxScore = MGPS->TotalScore;
			}
			else if (MGPS->TotalScore == MaxScore)
			{
				BombNominee.Add(MGPC);
			}
		}
	}

	// 후보자 무작위에게 폭탄 생성 후 부착
	int32 BombIndex = FMath::RandRange(0, BombNominee.Num() - 1);
	if (BombNominee.IsValidIndex(BombIndex))
	{
		BombActor->ActivateBomb(BombNominee[BombIndex]->GetCharacter(), ExplodeTime);
	}
}
