// MGGameInstance.cpp

#include "GameInstance/MGGameInstance.h"

#include "Type/MGPlayerColor.h"						// 플레이어 컬러

//사운드
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Data/MGMinigameInfoRow.h"

UMGGameInstance::UMGGameInstance()
{
	// GameInstance 최초 생성시 
	CurrentRoundState = ERoundState::Lobby;
}

void UMGGameInstance::Init()
{
	Super::Init();
	
	FWorldDelegates::OnSeamlessTravelStart.AddUObject(
		this, &UMGGameInstance::HandleSeamlessTravelStart);

	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
		this, &UMGGameInstance::HandlePostLoadMap);
}

FString UMGGameInstance::GetLevelURLForRound(int32 RoundIndex) const
{
	if (MinigameSequence.IsValidIndex(RoundIndex) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("[GameInstance] GetLevelURLForRound Failed! Invalid Index: %d / Array Size: %d"), RoundIndex, MinigameSequence.Num());
		return FString();
	}

	// TMap에서 맵 URL 찾기
	auto GameKey = MinigameSequence[RoundIndex];
	const FString* Level = MinigameLevels.Find(GameKey);

	if (Level == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[GameInstance] GetLevelURLForRound Failed! TMap does not contain Key at RoundIndex: %d"), RoundIndex);
		return FString();
	}

	return *Level;
}

FString UMGGameInstance::GetLevelURLForRoundState(ERoundState Round) const
{
	return GetLevelURLForRound(static_cast<int32>(Round) - static_cast<int32>(ERoundState::Round1));
}

void UMGGameInstance::HandleSeamlessTravelStart(UWorld* CurrentWorld, const FString& LevelName)
{
	// "/Game/.../L_MG_01_BombTag" -> "L_MG_01_BombTag" (RowName과 매칭용)
	PendingDestinationMapName = FPackageName::GetShortName(LevelName);

	UE_LOG(LogTemp, Warning, TEXT("[OnSeamlessTravelStart] Full=%s | Short=%s"),
		*LevelName, *PendingDestinationMapName);

	PlayBGM(nullptr, 0.5f);
}

#pragma region Sound

void UMGGameInstance::Shutdown()
{
	FWorldDelegates::OnSeamlessTravelStart.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);

	Super::Shutdown();
}

void UMGGameInstance::OnStart()
{
	Super::OnStart();

	// 최초 맵(타이틀)은 PostLoadMapWithWorld가 안 불릴 수 있으므로 수동 호출
	HandlePostLoadMap(GetWorld());
}

bool UMGGameInstance::IsAudioContext() const
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		return false;
	}
	return World->GetNetMode() != NM_DedicatedServer;
}

FName UMGGameInstance::GetCurrentMapName() const
{
	UWorld* World = GetWorld();
	if (!ensure(IsValid(World)))
	{
		return NAME_None;
	}

	FString MapName = World->GetMapName();
	MapName.RemoveFromStart(World->StreamingLevelsPrefix);
	return FName(*MapName);
}

void UMGGameInstance::HandlePostLoadMap(UWorld* LoadedWorld)
{
	if (IsAudioContext() == false)
	{
		return;
	}

	const FName MapName = GetCurrentMapName();

	if (ensure(IsValid(MinigameInfoTable)))
	{
		if (MinigameInfoTable->FindRow<FMGMinigameInfoRow>(MapName, TEXT("HandlePostLoadMap"), false) != nullptr)
		{
			PlayTutorialBGM();
			return;
		}
	}

	const TObjectPtr<USoundBase>* Found = LevelBGMs.Find(MapName);
	PlayBGM(Found != nullptr ? Found->Get() : nullptr);
}

void UMGGameInstance::PlayCurrentLevelBGM()
{
	const FName MapName = GetCurrentMapName();
	const TObjectPtr<USoundBase>* Found = LevelBGMs.Find(MapName);

	PlayBGM(Found != nullptr ? Found->Get() : nullptr);
}

void UMGGameInstance::PlayTutorialBGM()
{

	PlayBGM(TutorialBGM,0.5f);
}

void UMGGameInstance::PlayBGM(USoundBase* NewBGM, float FadeTime)
{
	if (IsAudioContext() == false)
	{
		return;
	}

	// 같은 곡이 이미 재생 중이면 무시 (재시작/중복 방지)
	if (LastRequestedBGM == NewBGM)
	{
		return;
	}
	LastRequestedBGM = NewBGM;

	if (IsValid(BGMComponent) == true && BGMComponent->IsPlaying() == true)
	{
		BGMComponent->bAutoDestroy = true;
		BGMComponent->FadeOut(FadeTime, 0.0f);
	}
	BGMComponent = nullptr;

	// 새 곡 페이드 인
	if (IsValid(NewBGM) == true)
	{
		BGMComponent = UGameplayStatics::SpawnSound2D(
			this, NewBGM,
			1.0f,      
			1.0f,     
			0.0f,      
			nullptr,   
			true,      
			false);    

		ensureMsgf(IsValid(BGMComponent), TEXT("SpawnSound2D failed for '%s'"), *NewBGM->GetName());

		if (IsValid(BGMComponent) == true)
		{
			BGMComponent->FadeIn(FadeTime, 1.0f);
		}
	}
}

#pragma endregion