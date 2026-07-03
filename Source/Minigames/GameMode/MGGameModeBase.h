// MGGameModeBase.h

#pragma once

#include "GameFramework/GameModeBase.h"
#include "MGGameModeBase.generated.h"

class AMGPlayerState;
class AMGPlayerController;

/**
 *
 */
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


public:
	FTimerHandle MainTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WaitingTime = 10;

	int32 RemainWaitingTimeForPlaying = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EndingTime = 15;

	int32 RemainWaitingTimeForEnding = 15;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AMGPlayerController>> AllPlayerControllers;
};
