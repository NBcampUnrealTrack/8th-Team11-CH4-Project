// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/MGPassBombGameMode.h"
#include "Controller/MGPlayerController.h"
#include "Character/MGPlayerCharacter.h"
#include "PlayerState/MGPlayerState.h"
#include "Gimmick/MGBombActor.h"

void AMGPassBombGameMode::StartMinigame()
{
	Super::StartMinigame();

	BombTime = 15.f;

	TArray<AMGPlayerController*> BombNominee;

	int32 MaxScore = -1;
	for (const auto MGPC : AllPlayerControllers)
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

	int32 BombIndex = FMath::RandRange(0, BombNominee.Num() - 1);
	if (BombNominee.IsValidIndex(BombIndex))
	{
		BombActor = GetWorld()->SpawnActor<AMGBombActor>(BombActorClass);
		BombActor->SetBombHolder(BombNominee[BombIndex]->GetCharacter());
		//BombPC = BombNominee[BombIndex];
		// BombNominee[BombIndex]->GetCharacter();
	}
}

void AMGPassBombGameMode::BeginPlay()
{
	Super::BeginPlay();
}
