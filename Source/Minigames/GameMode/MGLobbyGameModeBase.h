// Fill out your copyright notice in the Description page of Project Settings.

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
