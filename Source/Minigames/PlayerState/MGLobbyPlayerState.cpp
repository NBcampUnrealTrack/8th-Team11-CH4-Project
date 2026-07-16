// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/MGLobbyPlayerState.h"
#include "PlayerState/MGPlayerState.h"

#include "GameState/MGLobbyGameStateBase.h"
#include "Net/UnrealNetwork.h"

void AMGLobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMGLobbyPlayerState, bIsReady);
	DOREPLIFETIME(AMGLobbyPlayerState, PlayerColor);
}

void AMGLobbyPlayerState::SetReady(bool bReady)
{
	bIsReady = bReady;

	OnRep_IsReady();   // 데디에선 비용 0, 리슨 서버 보험
}

void AMGLobbyPlayerState::SetPlayerColor(EMGPlayerColor NewColor)
{
	PlayerColor = NewColor;

	OnRep_PlayerColor();
}

void AMGLobbyPlayerState::OnRep_IsReady()
{
	if (auto* LGS = GetWorld()->GetGameState<AMGLobbyGameStateBase>())
	{
		LGS->OnLobbyRosterChanged.Broadcast();
	}
}

void AMGLobbyPlayerState::OnRep_PlayerColor()
{
	if (auto* LGS = GetWorld()->GetGameState<AMGLobbyGameStateBase>())
	{
		LGS->OnLobbyRosterChanged.Broadcast();
	}
}

void AMGLobbyPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
	
	if (auto* LGS = GetWorld()->GetGameState<AMGLobbyGameStateBase>())
	{
		LGS->OnLobbyRosterChanged.Broadcast();
	}
}
