// MGGameInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Type/MGTypes.h"
#include "Type/MGChatType.h"
#include "Type/MGPlayerColor.h"
#include "MGGameInstance.generated.h"

class UDataTable;

struct FMGPlayerSaveData
{
	EMGPlayerColor Color = EMGPlayerColor::None;
	int32          TotalScore = 0;
	TArray<int32>  RoundScores;
};

UCLASS()
class MINIGAMES_API UMGGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMGGameInstance();
	
	virtual void Init() override;
	
	void SavePlayerData(APlayerState* PS);
	void RestorePlayerData(APlayerState* PS);
	
	
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
	TArray<FMGChatType> ChatMessageHistory;

	UPROPERTY()
	bool bChatVisible = false;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	FString PlayerNickname;
	
	UPROPERTY()
	TSet<FString> LobbyPlayerIds;
	
	UPROPERTY(BlueprintReadOnly, Category = "Loading")
	FString PendingDestinationMapName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loading")
	TObjectPtr<UDataTable> MinigameInfoTable;

	// 서버 전용 저장고 → UObject 포인터 없고 수명이 GI에 묶여있어 UPROPERTY 불필요
	TMap<FUniqueNetIdRepl, FMGPlayerSaveData> SavedPlayerData;
	
private:
	void HandleSeamlessTravelStart(UWorld* CurrentWorld, const FString& LevelName);

};
