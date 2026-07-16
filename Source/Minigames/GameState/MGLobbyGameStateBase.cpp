// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/MGLobbyGameStateBase.h"
#include "Net/UnrealNetwork.h"

void AMGLobbyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMGLobbyGameStateBase, CurrentPlayerCount);
	DOREPLIFETIME(AMGLobbyGameStateBase, RemainCountdownTime);
}

void AMGLobbyGameStateBase::SetCurrentPlayerCount(int32 NewCount)
{
	CurrentPlayerCount = NewCount;
	OnRep_HeaderChanged();
}

void AMGLobbyGameStateBase::SetRemainCountdownTime(int32 NewTime)
{
	RemainCountdownTime = NewTime;
	OnRep_HeaderChanged();
}

void AMGLobbyGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	
	OnLobbyRosterChanged.Broadcast();
}

void AMGLobbyGameStateBase::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	
	OnLobbyRosterChanged.Broadcast();
}

void AMGLobbyGameStateBase::OnRep_HeaderChanged()
{
	OnLobbyHeaderChanged.Broadcast();
}
