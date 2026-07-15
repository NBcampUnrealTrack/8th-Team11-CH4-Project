// MGGameInstance.cpp

#include "GameInstance/MGGameInstance.h"

#include "Engine/Engine.h"
#include "GameFramework/GameStateBase.h"
#include "PlayerState/MGLobbyPlayerState.h"
#include "PlayerState/MGPlayerState.h"
#include "Type/MGPlayerColor.h"						// 플레이어 컬러
#include "MGNetConfig.h"

UMGGameInstance::UMGGameInstance()
{
	// GameInstance 최초 생성시 
	CurrentRoundState = ERoundState::Lobby;
}

void UMGGameInstance::Init()
{
	Super::Init();

#if !MG_USE_EOS
	// IP 모드: ini의 EOS 넷드라이버를 IpNetDriver로 교체 (리슨 서버 생성 전 시점)
	if (GEngine != nullptr)
	{
		for (FNetDriverDefinition& Def : GEngine->NetDriverDefinitions)
		{
			if (Def.DefName == FName(TEXT("GameNetDriver")))
			{
				Def.DriverClassName         = FName(TEXT("/Script/OnlineSubsystemUtils.IpNetDriver"));
				Def.DriverClassNameFallback = FName(TEXT("/Script/OnlineSubsystemUtils.IpNetDriver"));
			}
		}
	}
#endif

	FWorldDelegates::OnSeamlessTravelStart.AddUObject(
		this, &UMGGameInstance::HandleSeamlessTravelStart);
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
	PendingDestinationMapName = FPackageName::GetShortName(LevelName);

	UE_LOG(LogTemp, Verbose, TEXT("[OnSeamlessTravelStart] Full=%s | Short=%s"),
	   *LevelName, *PendingDestinationMapName);

	if (CurrentWorld == nullptr || CurrentWorld->GetNetMode() == NM_Client)
	{
		return;
	}

	AGameStateBase* GS = CurrentWorld->GetGameState();
	if (IsValid(GS) == false)
	{
		return;
	}

	for (APlayerState* PS : GS->PlayerArray)
	{
		SavePlayerData(PS);
	}
}


void UMGGameInstance::SavePlayerData(APlayerState* PS)
{
	if (IsValid(PS) == false)
	{
		return;
	}

	const FUniqueNetIdRepl Id = PS->GetUniqueId();
	if (Id.IsValid() == false)
	{
		return;
	}

	FMGPlayerSaveData& Data = SavedPlayerData.FindOrAdd(Id);

	if (const AMGPlayerState* MGPS = Cast<AMGPlayerState>(PS))
	{
		Data.Color = MGPS->PlayerColor;
		Data.TotalScore = MGPS->TotalScore;
		Data.RoundScores = MGPS->RoundScores;

		UE_LOG(LogTemp, Warning, TEXT("[SaveData] %s | Score=%d | Rounds=%d | Color=%d"),
			*Id.ToString(), Data.TotalScore, Data.RoundScores.Num(), (int32)Data.Color);
	}
	else if (const AMGLobbyPlayerState* LPS = Cast<AMGLobbyPlayerState>(PS))
	{
		Data.Color = LPS->PlayerColor;   // 로비엔 점수 없음 → 색만 갱신
	}
}

void UMGGameInstance::RestorePlayerData(APlayerState* PS)
{
	if (IsValid(PS) == false)
	{
		return;
	}

	const FUniqueNetIdRepl Id = PS->GetUniqueId();
	if (Id.IsValid() == false)
	{
		return;
	}

	const FMGPlayerSaveData* Data = SavedPlayerData.Find(Id);
	if (Data == nullptr)
	{
		return;   // 저장된 게 없으면(최초 접속) 손대지 않음
	}

	if (AMGPlayerState* MGPS = Cast<AMGPlayerState>(PS))
	{
		MGPS->SetPlayerColor(Data->Color);
		MGPS->TotalScore = Data->TotalScore;
		MGPS->RoundScores = Data->RoundScores;

		UE_LOG(LogTemp, Warning, TEXT("[RestoreData] %s | Score=%d | Rounds=%d"),
			*Id.ToString(), Data->TotalScore, Data->RoundScores.Num());
	}
	else if (AMGLobbyPlayerState* LPS = Cast<AMGLobbyPlayerState>(PS))
	{
		LPS->SetPlayerColor(Data->Color);
	}
}
