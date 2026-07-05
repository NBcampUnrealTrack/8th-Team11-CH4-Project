// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/MGPassBombGameMode.h"
#include "Controller/MGPlayerController.h"
#include "Character/MGPlayerCharacter.h"
#include "PlayerState/MGPassBombPlayerState.h"
#include "Gimmick/MGBombActor.h"

void AMGPassBombGameMode::StartMinigame()
{
	Super::StartMinigame();

	ExplodeTime = 5.f;

	AlivePlayer = AllPlayerControllers;

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
	AController* PC = TargetPlayer->GetController();
	if (IsValid(PC))
	{
		AMGPlayerController* MGPC = Cast<AMGPlayerController>(PC);
		if (IsValid(MGPC))
		{
			if (AlivePlayer.Contains(MGPC))
			{
				AMGPassBombPlayerState* MGPS = MGPC->GetPlayerState<AMGPassBombPlayerState>();
				if (IsValid(MGPS))
				{
					MGPS->MulticastRPC_RetireCharacter();
				}

				//TODO: 중간순위 점수 추가
				//MGPC->AddScore()

				AlivePlayer.Remove(MGPC);
			}
			
			
			// 플레이어명 임시 지정, 이후 변경필요
			FString UserName;
			UserName = MGPC->GetPlayerState<AMGPassBombPlayerState>()->GetPlayerName();

			NotifyToAllPlayer(FString::Printf(TEXT("%s(이)가 탈락했습니다!"), *UserName));
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
	if (AlivePlayer.Num() <= 1)
	{
		// 플레이어명 임시 지정, 이후 변경필요
		FString UserName;
		UserName = AlivePlayer[0]->GetPlayerState<AMGPassBombPlayerState>()->GetPlayerName();

		NotifyToAllPlayer(FString::Printf(TEXT("%s 승리!"), *UserName));

		//TODO: 승리자 점수 추가
		//AlivePlayer[0]->AddScore()

		EndMinigame();
		return;
	}
	NotifyToAllPlayer(TEXT(""));

	// 술래 후보자
	TArray<AMGPlayerController*> BombNominee;

	// 점수가 가장 높은 사람들을 후보로 지명
	int32 MaxScore = -1;
	for (const auto MGPC : AlivePlayer)
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
