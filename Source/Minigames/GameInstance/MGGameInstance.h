// MGGameInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameState/MGGameStateBase.h"
#include "Type/MGTypes.h"
#include "Type/MGChatType.h"
#include "MGGameInstance.generated.h"

enum class EMGPlayerColor : uint8;
class UDataTable;

// 레벨 사운드
class UAudioComponent;
class USoundBase;

UCLASS()
class MINIGAMES_API UMGGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMGGameInstance();
	
	virtual void Init() override;

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
	
	UPROPERTY(BlueprintReadOnly, Category = "Loading")
	FString PendingDestinationMapName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loading")
	TObjectPtr<UDataTable> MinigameInfoTable;
	
private:
	void HandleSeamlessTravelStart(UWorld* CurrentWorld, const FString& LevelName);

#pragma region Sound
public:
	virtual void Shutdown() override;
	virtual void OnStart() override;

	// 페이드로 BGM 교체. nullptr을 주면 페이드 아웃만.
	UFUNCTION(BlueprintCallable, Category = "Audio|BGM")
	void PlayBGM(USoundBase* NewBGM, float FadeTime = 1.0f);

	// 현재 레벨
	UFUNCTION(BlueprintCallable, Category = "Audio|BGM")
	void PlayCurrentLevelBGM();

	// 미니게임 설명 UI 
	UFUNCTION(BlueprintCallable, Category = "Audio|BGM")
	void PlayTutorialBGM();

	// 컷씬
	UFUNCTION(BlueprintCallable, Category = "Audio|BGM")
	void PlayCutsceneBGM();


protected:
	// 레벨 로드 완료 시 자동 호출
	void HandlePostLoadMap(UWorld* LoadedWorld);

	// 현재 맵 이름
	FName GetCurrentMapName() const;

	// 레벨별 곡. 키 = 맵 이름.
	UPROPERTY(EditDefaultsOnly, Category = "Audio|BGM")
	TMap<FName, TObjectPtr<USoundBase>> LevelBGMs;

	// 미니게임 설명 UI
	UPROPERTY(EditDefaultsOnly, Category = "Audio|BGM")
	TObjectPtr<USoundBase> TutorialBGM;

	//컷씬
	UPROPERTY(EditDefaultsOnly, Category = "Audio|BGM")
	TObjectPtr<USoundBase> CutsceneBGM;

	UPROPERTY()
	TObjectPtr<UAudioComponent> BGMComponent;

private:
	FDelegateHandle PostLoadMapHandle;

	// 오디오를 실제로 재생해야 하는 컨텍스트인지 (데디 서버 제외용 리슨 서버 문제 없음)
	bool IsAudioContext() const;
#pragma endregion
};