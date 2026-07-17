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

	// virtual void OnRep_Owner() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_RetireCharacter();

	// UFUNCTION(Server, Reliable)
	// void ServerRPC_SetSpectator();

	void SpawnSpectator();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Spectator")
	TSubclassOf<AMGSpectatorPawn> SpectatorClass;

	UPROPERTY(Replicated)
	AMGSpectatorPawn* Spectator;
};
