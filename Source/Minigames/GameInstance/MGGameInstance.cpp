// MGGameInstance.cpp

#include "GameInstance/MGGameInstance.h"

#include "Type/MGPlayerColor.h"						// 플레이어 컬러

UMGGameInstance::UMGGameInstance()
{
	// GameInstance 최초 생성시 
	CurrentRoundState = ERoundState::Lobby;
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
