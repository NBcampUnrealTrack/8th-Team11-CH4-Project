// MGFlagGameModeBase.cpp


#include "GameMode/MGFlagGameModeBase.h"

#include "Character/MGPlayerCharacter.h"
#include "Component/MGFlagActorComponent.h"
#include "Controller/MGPlayerController.h"
#include "GameState/MGFlagGameStateBase.h"
#include "PlayerState/MGFlagPlayerState.h"
#include "SpawnPoint/MGFlagSpawnPoint.h"
#include "Gimmick/MGFlagActor.h"
#include "Kismet/GameplayStatics.h"

void AMGFlagGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMGFlagGameModeBase::StartMinigame()
{
	Super::StartMinigame();
	
	GetWorld()->GetTimerManager().SetTimer(
		GameTimerHandle,
		this,
		&ThisClass::OnGameTimerElapsed,
		1.f,
		true
	);
	
	UE_LOG(LogTemp, Warning, TEXT("StartMinigame"));
	SpawnFlag();
}

void AMGFlagGameModeBase::EndMinigame()
{
	Super::EndMinigame();
	
	DetermineWinner();
	GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);
}

void AMGFlagGameModeBase::OnGameTimerElapsed()
{
	--RemainGameTime;
	if (AMGFlagGameStateBase* FGS = GetGameState<AMGFlagGameStateBase>())
	{
		FGS->RemainGameTime = RemainGameTime;
	}
	
	UpdateFlagHoldingTime();
	
	if (RemainGameTime == 0)
	{
		EndMinigame();
	}
}

void AMGFlagGameModeBase::DetermineWinner()
{
	TArray<AMGFlagPlayerState*> FlagPlayerStates;

	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (AMGFlagPlayerState* FlagPS = Cast<AMGFlagPlayerState>(PS))
		{
			FlagPlayerStates.Add(FlagPS);
		}
	}
	
	FlagPlayerStates.Sort(
		[](const AMGFlagPlayerState& A, const AMGFlagPlayerState& B)
		{
			return A.HoldingTime > B.HoldingTime;
		}
	);
	
	int32 Rank = 1;
	for (int32 i = 0; i < FlagPlayerStates.Num(); i++)
	{
		if (i > 0 && FlagPlayerStates[i]->HoldingTime < FlagPlayerStates[i-1]->HoldingTime)
		{
			Rank = i + 1;
		}
		FlagPlayerStates[i]->Rank = Rank;
		GiveScore(FlagPlayerStates[i], Rank);
	}
}

void AMGFlagGameModeBase::SpawnFlag()
{
	if (!FlagClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("FlagClass is not set!"));
		return;
	}
	
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMGFlagSpawnPoint::StaticClass(), SpawnPoints);
	if (SpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Flag SpawnPoints found!"));
		return;
	}
	
	int32 RandPoint = FMath::RandRange(0, SpawnPoints.Num() - 1);
	FTransform SpawnTransform = SpawnPoints[RandPoint]->GetActorTransform();
	GetWorld()->SpawnActor<AMGFlagActor>(FlagClass, SpawnTransform);
	
	UE_LOG(LogTemp, Warning, TEXT("Flag spawned at %s"), *SpawnTransform.GetLocation().ToString());
}

void AMGFlagGameModeBase::UpdateFlagHoldingTime()
{
	for (AMGPlayerController* PC : AllPlayerControllers)
	{
		AMGPlayerCharacter* Character = Cast<AMGPlayerCharacter>(PC->GetPawn());
		if (!IsValid(Character))
		{
			continue;
		}
		
		UMGFlagActorComponent* FlagComp = Character->GetComponentByClass<UMGFlagActorComponent>();
		if (!IsValid(FlagComp) || FlagComp->GetHasFlag() == false)
		{
			continue;
		}
		
		AMGFlagPlayerState* FlagPS = PC->GetPlayerState<AMGFlagPlayerState>();
		if (!IsValid(FlagPS))
		{
			continue;
		}
		
		FlagPS->HoldingTime += 1.f;
		
		UE_LOG(LogTemp, Warning, TEXT("[Flag] Owner: %s | HoldingTime: %.1f"),
			*FlagPS->GetPlayerName(), FlagPS->HoldingTime);
	}
}

void AMGFlagGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	GetWorld()->GetTimerManager().ClearTimer(GameTimerHandle);
}
