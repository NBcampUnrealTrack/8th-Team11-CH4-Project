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

	virtual void BeginPlay() override;

protected:
	FTimerHandle BombTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minigame|Rule")
	int32 BombTime;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//TObjectPtr<AMGPlayerController> BombPC;

	UPROPERTY(EditDefaultsOnly, Category = "Minigame|Rule")
	TSubclassOf<class AMGBombActor> BombActorClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minigame|Rule")
	TObjectPtr<class AMGBombActor> BombActor;
};