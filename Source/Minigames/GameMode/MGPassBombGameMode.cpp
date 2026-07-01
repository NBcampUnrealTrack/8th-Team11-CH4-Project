// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/MGPassBombGameMode.h"
#include "Controller/MGPlayerController.h"
#include "Character/MGPlayerCharacter.h"
#include "PlayerState/MGPlayerState.h"
#include "Component/PassBomb/MGCharacterComp_PassBomb.h"
#include "Gimmick/MGBombActor.h"

void AMGPassBombGameMode::StartMinigame()
{
	Super::StartMinigame();

	BombTime = 15.f;

	// 액터컴포넌트 부착
	for (const auto MGPC : AllPlayerControllers)
	{
		ACharacter* PlayerChar = MGPC->GetCharacter();
		if (IsValid(PlayerChar))
		{
			AMGPlayerCharacter* MGPlayerChar = Cast<AMGPlayerCharacter>(PlayerChar);
			if (IsValid(MGPlayerChar))
			{
				MGPlayerChar->MulticastRPC_SetRule(UMGCharacterComp_PassBomb::StaticClass());
			}
		}
	}

	AlivePlayer = AllPlayerControllers;

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
		BombActor = GetWorld()->SpawnActor<AMGBombActor>(BombActorClass);
		BombActor->ActivateBomb(BombNominee[BombIndex]->GetCharacter());
	}
}

void AMGPassBombGameMode::BeginPlay()
{
	Super::BeginPlay();
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
				AlivePlayer.Remove(MGPC);
			}
		}
	}
}
