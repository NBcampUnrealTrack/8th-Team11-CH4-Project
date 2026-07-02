// MGGameModeBase.h

#pragma once

#include "GameFramework/GameModeBase.h"
#include "MGGameModeBase.generated.h"

class AMGPlayerState;
class AMGPlayerController;

UENUM(BlueprintType)
enum class EMinigameType : uint8
{
	PASSBOME UMETA(DisplayName = "PassTheBomb")
};
/**
 *
 */
UCLASS()
class MINIGAMES_API AMGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMGGameModeBase();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	virtual void BeginPlay() override;

	virtual void StartMinigame();	//오버라이드 시 Super호출 필수

	virtual void EndMinigame();		//오버라이드 시 Super호출 필수

	void OnCharacterDead(AMGPlayerController* InController);

	void GiveScore(AMGPlayerState* PS, int32 Rank);

protected:
	void NotifyToAllPlayer(const FString& NotificationString);
	
private:
	UFUNCTION()
	void OnMainTimerElapsed();


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
