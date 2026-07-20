// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/MGGameModeBase.h"
#include "MGPassBombGameMode.generated.h"

/**
 * 
 */

class AMGBombActor;
class AMGPlayerController;
UCLASS()
class MINIGAMES_API AMGPassBombGameMode : public AMGGameModeBase
{
	GENERATED_BODY()

public:
	virtual void StartMinigame() override;

	virtual void EndMinigame() override;

	virtual void BeginPlay() override;

	virtual void Logout(AController* Exiting) override;
	
	void EliminatePlayer(ACharacter* TargetPlayer);

	UFUNCTION()
	void NextRound();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	// 폭탄 카운트다운이 시작된 후 터질 때까지 걸리는 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minigame|Rule")
	int32 InitExplodeTime = 20.f;

	int32 ExplodeTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minigame|Rule")
	TObjectPtr<AMGBombActor> BombActor;

	// 라운드 전환 타이머
	FTimerHandle RoundTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Minigame|Rule")
	TSubclassOf<AMGBombActor> BombActorClass;

	UPROPERTY(BlueprintReadOnly, Category = "Minigame")
	TArray<TObjectPtr<AMGPlayerController>> AlivePlayers;

private:
	bool TryFinishByLastManStanding();
	
	void AssignBombToRandomAlive();

	UFUNCTION()
	void SetBombActorCollisionEnabled(bool Value);
	
	FTimerHandle CutSceneTimerHandler;
};
