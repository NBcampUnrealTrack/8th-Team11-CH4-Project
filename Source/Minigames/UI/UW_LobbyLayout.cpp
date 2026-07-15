// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_LobbyLayout.h"

#include "UW_LobbyPlayerRow.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Controller/MGPlayerController.h"
#include "GameState/MGLobbyGameStateBase.h"
#include "PlayerState/MGLobbyPlayerState.h"

void UUW_LobbyLayout::NativeConstruct()
{
	Super::NativeConstruct();

	// 1) 버튼 OnClicked 바인딩 (dynamic → AddDynamic)
	ReadyButton->OnClicked.AddDynamic(this, &ThisClass::OnReadyClicked);

	// 2) GameState 델리게이트 구독 (non-dynamic → AddUObject)
	if (AMGLobbyGameStateBase* LGS = GetLobbyGameState())
	{
		CachedGameState = LGS; // LGS 보관
		LGS->OnLobbyRosterChanged.AddUObject(this, &ThisClass::RefreshRoster);
		LGS->OnLobbyHeaderChanged.AddUObject(this, &ThisClass::RefreshHeader);
	}

	// 3) 초기 1회 — 이미 복제돼 있던 상태 반영 (타이밍 함정 대응)
	RefreshRoster();
	RefreshHeader();
}

void UUW_LobbyLayout::NativeDestruct()
{
	if (AMGLobbyGameStateBase* GS = CachedGameState.Get())
	{
		GS->OnLobbyRosterChanged.RemoveAll(this);
		GS->OnLobbyHeaderChanged.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UUW_LobbyLayout::OnReadyClicked()
{
	AMGPlayerController* PC = GetOwningPlayer<AMGPlayerController>();
	if (!IsValid(PC))
	{
		return;
	}

	bool bCurrentReady = false;
	if (auto* PS = PC->GetPlayerState<AMGLobbyPlayerState>())
	{
		bCurrentReady = PS->bIsReady;
	}

	PC->ServerRPCSetReady(!bCurrentReady);
}

void UUW_LobbyLayout::OnColorClicked()
{
	// TODO
}

void UUW_LobbyLayout::RefreshHeader()
{
    AMGLobbyGameStateBase* GS = GetLobbyGameState();
    if (!GS)
    {
        return;
    }

    // 인원수 "N/10"
    PlayerCountText->SetText(FText::Format(
        NSLOCTEXT("Lobby", "PlayerCount", "{0}/10"),
        FText::AsNumber(GS->GetCurrentPlayerCount()))
    );

    // 카운트다운 중이면 숫자+잠금, 아니면 READY+활성
    if (GS->GetRemainCountdownTime() > 0)
    {
        CountdownText->SetText(FText::AsNumber(GS->GetRemainCountdownTime()));
        ReadyButton->SetIsEnabled(false);
    }
    else
    {
        CountdownText->SetText(NSLOCTEXT("Lobby", "Ready", "READY"));
        ReadyButton->SetIsEnabled(true);
    }
}

void UUW_LobbyLayout::RefreshRoster()
{
	AMGLobbyGameStateBase* GS = GetLobbyGameState();
	if (!GS || !PlayerRowClass)
	{
		return;
	}

	// PlayerArray는 클라마다 순서가 달라 → 복사 후 PlayerId로 정렬 (모든 클라 동일 순서)
	TArray<APlayerState*> Players = GS->PlayerArray;
	Players.Sort([](const APlayerState& A, const APlayerState& B)
	{
		return A.GetPlayerId() < B.GetPlayerId();
	});
	
	int32 ReadyCount = 0;
	
	for (int32 i = 0; i < Players.Num(); ++i)
	{
		AMGLobbyPlayerState* PS = Cast<AMGLobbyPlayerState>(Players[i]);
		if (!PS)
		{
			continue;
		}
		
		if (PS->bIsReady)
		{
			++ReadyCount;
		}
		
		// 있으면 재사용, 없으면 새로 만들어 붙임
		if (!RowWidgets.IsValidIndex(i))
		{
			UUW_LobbyPlayerRow* Row = CreateWidget<UUW_LobbyPlayerRow>(this, PlayerRowClass);
			PlayerListBox->AddChild(Row);
			RowWidgets.Add(Row);
		}
		RowWidgets[i]->Setup(PS);   // 색/레디/이름 반영 (6단계 위젯)
	}
	
	ReadyCountText->SetText(FText::Format(
	   NSLOCTEXT("Lobby", "ReadyCount", "{0}명 준비완료"),
	   FText::AsNumber(ReadyCount))
	);

	for (int32 i = RowWidgets.Num() - 1; i >= Players.Num(); --i)
	{
		RowWidgets[i]->RemoveFromParent();
		RowWidgets.RemoveAt(i);
	}
}

AMGLobbyGameStateBase* UUW_LobbyLayout::GetLobbyGameState() const
{
	return GetWorld() ? GetWorld()->GetGameState<AMGLobbyGameStateBase>() : nullptr;
}
