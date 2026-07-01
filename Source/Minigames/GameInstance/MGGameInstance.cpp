// MGGameInstance.cpp

#include "GameInstance/MGGameInstance.h"

UMGGameInstance::UMGGameInstance()
{
	// GameInstance 최초 생성시 
	CurrentRoundState = ERoundState::Lobby;
}

//void UMGGameInstance::Init()
//{
//	Super::Init();
//}

FString UMGGameInstance::GetLevelURLForRound(int32 RoundIndex) const
{
	if (MinigameSequence.IsValidIndex(RoundIndex) == false)
	{
		return FString();
	}
	const FString* Level = MinigameLevels.Find(MinigameSequence[RoundIndex]);
	return (Level != nullptr) ? *Level : FString();
}

FString UMGGameInstance::GetLevelURLForRoundState(ERoundState Round) const
{
	return GetLevelURLForRound(static_cast<int32>(Round) - static_cast<int32>(ERoundState::Round1));
}
