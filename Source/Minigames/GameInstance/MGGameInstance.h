// MGGameInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameState/MGGameStateBase.h"
#include "Type/MGTypes.h"
#include "Type/MGChatType.h"
#include "MGGameInstance.generated.h"

enum class EMGPlayerColor : uint8;

UCLASS()
class MINIGAMES_API UMGGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMGGameInstance();

	UFUNCTION(BlueprintPure, Category = "Game Flow")
	FString GetLevelURLForRound(int32 RoundIndex) const;

	UFUNCTION(BlueprintPure, Category = "Game Flow")
	FString GetLevelURLForRoundState(ERoundState Round) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Flow")
	ERoundState CurrentRoundState;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Flow")
	TMap<EMinigameType, FString> MinigameLevels;
	
	UPROPERTY(BlueprintReadOnly, Category = "Game Flow")
	TArray<EMinigameType> MinigameSequence;
	
	UPROPERTY()
	TMap<FUniqueNetIdRepl, EMGPlayerColor> PlayerColors;

	UPROPERTY()
	TArray<FMGChatType> ChatMessageHistory;

	UPROPERTY()
	bool bChatVisible = false;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	FString PlayerNickname;

};
