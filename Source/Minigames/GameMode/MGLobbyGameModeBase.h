// MGLobbyGameModeBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MGLobbyGameModeBase.generated.h"

class AMGPlayerController;
/**
 * 
 */
UCLASS()
class MINIGAMES_API AMGLobbyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMGLobbyGameModeBase();
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* ExitingController) override;
	
	void OnPlayerReady(AMGPlayerController* PC, bool bReady);

	// 미니게임 종료 후 Seamless를 통해 돌아오는 플레이어들을 처리하기 위한 함수 
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinimumPlayerCount = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 CountdownTime = 10;

	// UPROPERTY(EditDefaultsOnly)
	// FString MinigameLevelName;
	
	UPROPERTY(EditDefaultsOnly)
	int32 TotalRoundCount = 3;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AMGPlayerController>> AllPlayerControllers;
	
private:
	UFUNCTION()
	void OnCountdownElapsed();
	
	void CheckAndStartCountdown();
	
	void CancelCountdown();
	
	void TravelToMinigameLevel();
	
	void GenerateMinigameSequence();
	
	bool CheckAllPlayersReady();
	
	FTimerHandle CountdownTimerHandle;

};
