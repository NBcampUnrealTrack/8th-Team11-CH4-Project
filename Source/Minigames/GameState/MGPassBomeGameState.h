// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameState/MGGameStateBase.h"
#include "MGPassBomeGameState.generated.h"

/**
 * 
 */
UCLASS()
class MINIGAMES_API AMGPassBomeGameState : public AMGGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	//UPROPERTY(Replicated, BlueprintReadOnly, Category = "Minigame")
	//TObjectPtr<> BombPC;
};
