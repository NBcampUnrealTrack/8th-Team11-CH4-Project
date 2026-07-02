// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/MGLobbyGameStateBase.h"
#include "Net/UnrealNetwork.h"

void AMGLobbyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMGLobbyGameStateBase, CurrentPlayerCount);
	DOREPLIFETIME(AMGLobbyGameStateBase, RemainCountdownTime);
}
