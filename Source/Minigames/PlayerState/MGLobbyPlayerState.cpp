// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/MGLobbyPlayerState.h"

#include "Net/UnrealNetwork.h"

void AMGLobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMGLobbyPlayerState, bIsReady);
}

void AMGLobbyPlayerState::SetReady(bool bReady)
{
	bIsReady = bReady;

	// TODO: 레디 상태 UI 갱신 (호스트/서버 화면용)
}

void AMGLobbyPlayerState::OnRep_IsReady()
{
	// TODO: 클라이언트 레디 상태 UI 갱신
}

