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

	void OnCharacterDead(AMGPlayerController* InController);

	void GiveScore(AMGPlayerState* PS, int32 Rank);
	
private:
	UFUNCTION()
	void OnMainTimerElapsed();

	void NotifyToAllPlayer(const FString& NotificationString);

public:
	FTimerHandle MainTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WaitingTime = 15;

	int32 RemainWaitingTimeForPlaying = 15;

	int32 MinimumPlayerCountForPlaying = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EndingTime = 15;

	int32 RemainWaitingTimeForEnding = 15;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AMGPlayerController>> AllPlayerControllers;
};
