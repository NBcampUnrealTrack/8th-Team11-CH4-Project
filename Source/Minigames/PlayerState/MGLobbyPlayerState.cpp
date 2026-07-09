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

void AMGLobbyPlayerState::OnRep_IsReady()
{
	// 클라이언트 레디 상태 UI 갱신
	if (auto* LGS = GetWorld()->GetGameState<AMGLobbyGameStateBase>())
	{
		LGS->OnLobbyRosterChanged.Broadcast();
	}
}

void AMGLobbyPlayerState::OnRep_PlayerColor()
{
	// 클라이언트 color 상태 UI 갱신
	if (auto* LGS = GetWorld()->GetGameState<AMGLobbyGameStateBase>())
	{
		LGS->OnLobbyRosterChanged.Broadcast();
	}
	
	// TestLog
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
	// 	FString::Printf(TEXT("[Player %d] Color: %d"), GetPlayerId(), (uint8)PlayerColor));
}

void AMGLobbyPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	// 로비 -> 미니게임 트래블 시 배정된 색을 새 PlayerState로 전달
	if (AMGPlayerState* NewPS = Cast<AMGPlayerState>(PlayerState))
	{
		NewPS->PlayerColor = this->PlayerColor;
	}
}