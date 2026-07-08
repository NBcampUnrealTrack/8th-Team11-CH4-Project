// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayerState/MGPlayerState.h"
#include "MGPassBombPlayerState.generated.h"

/**
 * 
 */
class AMGSpectatorPawn;

UCLASS()
class MINIGAMES_API AMGPassBombPlayerState : public AMGPlayerState
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_RetireCharacter();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Spectator")
	TSubclassOf<AMGSpectatorPawn> SpectatorClass;

	UPROPERTY()
	AMGSpectatorPawn* Spectator;
};
