// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/MGGameModeBase.h"
#include "MGPassBombGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MINIGAMES_API AMGPassBombGameMode : public AMGGameModeBase
{
	GENERATED_BODY()

public:
	virtual void StartMinigame() override;

	virtual void EndMinigame() override;

	virtual void BeginPlay() override;

	void EliminatePlayer(ACharacter* TargetPlayer);

	UFUNCTION()
	void NextRound();

protected:
	// 폭탄 카운트다운이 시작된 후 터질 때까지 걸리는 시간
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minigame|Rule")
	int32 ExplodeTime;

	// 라운드 전환 타이머
	FTimerHandle RoundTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Minigame|Rule")
	TSubclassOf<class AMGBombActor> BombActorClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minigame|Rule")
	TObjectPtr<class AMGBombActor> BombActor;
};
