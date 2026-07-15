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

		AMGPassBombPlayerState* MGPS = AlivePlayers[i]->GetPlayerState<AMGPassBombPlayerState>();
		if (IsValid(MGPS))
		{
			MGPS->SpawnSpectator();
		}
	}

	NextRound();
}

void AMGPassBombGameMode::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	BombActor = GetWorld()->SpawnActor<AMGBombActor>(BombActorClass,FTransform::Identity,SpawnParam);
}

// 중간 이탈자 방어 코드
void AMGPassBombGameMode::Logout(AController* Exiting)
{
	AMGPlayerController* MGPC = Cast<AMGPlayerController>(Exiting);
	if (IsValid(MGPC) && AlivePlayers.Contains(MGPC))
	{
		AMGPassBombGameState* MGGS = GetGameState<AMGPassBombGameState>();

		// 제거 전에 캐릭터 캡처 + 폭탄 보유자였는지 확인
		ACharacter* LeaverChar = MGPC->GetCharacter();
		const bool bWasBombHolder =
			IsValid(BombActor) && IsValid(BombActor->BombHolder) && (BombActor->BombHolder == LeaverChar);

		AlivePlayers.Remove(MGPC);
		if (IsValid(MGGS) && IsValid(LeaverChar))
		{
			MGGS->AliveCharacters.Remove(LeaverChar);
		}

		// 승리 재확인 → 아니면 폭탄 보유자였으면 재부여
		if (TryFinishByLastManStanding() == false)
		{
			if (bWasBombHolder)
			{
				AssignBombToRandomAlive();
			}
		}
	}

	Super::Logout(Exiting);
}

void AMGPassBombGameMode::EliminatePlayer(ACharacter* TargetPlayer)
{
	// 이미 종료됐으면(이탈로 승자 확정 등) 잔여 폭발 무시
	AMGGameStateBase* GS = GetGameState<AMGGameStateBase>();
	if (IsValid(GS) && GS->MatchState == EMatchState::Ending)
	{
		return;
	}
	
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

				const int32 Rank = AlivePlayers.Num();
				GiveScore(MGPS, Rank);

				AlivePlayers.Remove(MGPC);
				MGGS->AliveCharacters.Remove(TargetPlayer);
			}

			FString UserName = MGPC->GetPlayerState<AMGPassBombPlayerState>()->GetPlayerName();
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
	if (TryFinishByLastManStanding())
	{
		return;
	}
	NotifyToAllPlayer(TEXT(""));
	AssignBombToRandomAlive();
}

// 최후 1인이면 종료. 승자 확정 시 true 반환.
bool AMGPassBombGameMode::TryFinishByLastManStanding()
{
	if (AlivePlayers.Num() > 1)
	{
		return false;
	}

	// 예약된 다음 라운드 취소 (이탈로 먼저 끝나는 경우 대비)
	GetWorldTimerManager().ClearTimer(RoundTimerHandle);

	if (AlivePlayers.Num() == 1 && IsValid(AlivePlayers[0]))
	{
		FString UserName = AlivePlayers[0]->GetPlayerState<AMGPassBombPlayerState>()->GetPlayerName();
		NotifyToAllPlayer(FString::Printf(TEXT("%s 승리!"), *UserName));

		AMGPlayerState* WinnerPS = AlivePlayers[0]->GetPlayerState<AMGPlayerState>();
		if (IsValid(WinnerPS))
		{
			GiveScore(WinnerPS, 1);
		}
	}
	// Num()==0 (전원 이탈)이면 승자 없이 그냥 종료

	EndMinigame();
	return true;
}

void AMGPassBombGameMode::AssignBombToRandomAlive()
{
	int32 BombIndex = FMath::RandRange(0, AlivePlayers.Num() - 1);
	if (AlivePlayers.IsValidIndex(BombIndex))
	{
		AMGPlayerController* Target = AlivePlayers[BombIndex];
		if (IsValid(Target) && IsValid(Target->GetCharacter()))
		{
			BombActor->ActivateBomb(Target->GetCharacter(), ExplodeTime);
		}
	}
}

