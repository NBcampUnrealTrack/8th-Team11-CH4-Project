// MGGameModeBase.h

#pragma once

#include "GameFramework/GameModeBase.h"
#include "MGGameModeBase.generated.h"

class AMGPlayerState;
class AMGPlayerController;

UCLASS()
class MINIGAMES_API AMGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMGGameModeBase();

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	virtual void BeginPlay() override;

	virtual void StartMinigame();	//오버라이드 시 Super호출 필수

	virtual void EndMinigame();		//오버라이드 시 Super호출 필수

	// 로비에서 Seamless를 통해 돌아오는 플레이어들을 처리하기 위한 함수 
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	
	void OnCharacterDead(AMGPlayerController* InController);

	void GiveScore(AMGPlayerState* PS, int32 Rank);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	void NotifyToAllPlayer(const FString& NotificationString);

private:
	UFUNCTION()
	void OnMainTimerElapsed();

	void FillColorPlayers();

public:
	FTimerHandle MainTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WaitingTime = 8;
	int32 RemainWaitingTimeForPlaying = 8;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EndingTime = 10;
	int32 RemainWaitingTimeForEnding = 10;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AMGPlayerController>> AllPlayerControllers;
	
	UPROPERTY(EditDefaultsOnly, Category = "GameFlow")
	int32 EnteringMaxWaitTime = 30;   // 낙오자 대비 최대 대기(초)
	int32 RemainEnteringWaitTime = 30;

#pragma region CutScene

public:
	void PlayCutScene(); // 컷신 재생

	void OnFinishedCutScene(); // 컷신 재생 종료

private:
	FTimerHandle CutSceneTimerHandler;

	UPROPERTY(EditAnywhere, Category = "CutScene")
	bool bUseCutScene = true;

#pragma endregion

};
