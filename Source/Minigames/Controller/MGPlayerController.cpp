// MGPlayerController.cpp


#include "Controller/MGPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "GameMode/MGGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/UW_GameResult.h"
#include "Components/TextBlock.h"
#include "GameMode/MGLobbyGameModeBase.h" 
#include "Type/MGPlayerColor.h"
#include "UI/UW_LobbyLayout.h"
#include "GameState/MGLobbyGameStateBase.h"

void AMGPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController() == false)
	{
		return;
	}

	if (GetWorld()->GetGameState<AMGLobbyGameStateBase>() != nullptr)
	{
		if (IsValid(LobbyLayoutClass) == true)
		{
			if (UUW_LobbyLayout* Lobby = CreateWidget<UUW_LobbyLayout>(this, LobbyLayoutClass))
			{
				Lobby->AddToViewport();
			}
		}

		// 로비: 마우스로 UI 클릭 가능하게
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
	else
	{
		// 미니게임
		FInputModeGameOnly GameOnly;
		SetInputMode(GameOnly);
		bShowMouseCursor = false;
	}

	if (IsValid(NotificationTextUIClass) == true)
	{
		UUserWidget* NotificationTextUI = CreateWidget<UUserWidget>(this, NotificationTextUIClass);
		if (IsValid(NotificationTextUI) == true)
		{
			NotificationTextUI->AddToViewport(1);

			NotificationTextUI->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}

void AMGPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void AMGPlayerController::OnCharacterDead()
{
	AMGGameModeBase* GameMode = Cast<AMGGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (HasAuthority() == true && IsValid(GameMode) == true)
	{
		GameMode->OnCharacterDead(this);
	}
}

void AMGPlayerController::ClientRPCReturnToTitle_Implementation()
{
	if (IsLocalController() == true)
	{ // 서버의 레벨이 변경되는걸 원치 않음. 클라이언트가 이동해야하므로 if() 처리.
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Title")), true);
	}
}

void AMGPlayerController::ClientRPCShowGameResultWidget_Implementation(int32 InRanking)
{
	if (IsLocalController() == true)
	{
		if (IsValid(GameResultUIClass) == true)
		{
			UUW_GameResult* GameResultUI = CreateWidget<UUW_GameResult>(this, GameResultUIClass);
			if (IsValid(GameResultUI) == true)
			{
				GameResultUI->AddToViewport(3);

				FString GameResultString = FString::Printf(TEXT("%s"), InRanking == 1 ? TEXT("Winner Winner!") : TEXT("Looser..."));
				GameResultUI->ResultText->SetText(FText::FromString(GameResultString));

				FString RankingString = FString::Printf(TEXT("#%02d"), InRanking);
				GameResultUI->RankingText->SetText(FText::FromString(RankingString));

				FInputModeUIOnly Mode;
				Mode.SetWidgetToFocus(GameResultUI->GetCachedWidget());
				SetInputMode(Mode);

				bShowMouseCursor = true;
			}
		}
	}
}

void AMGPlayerController::ChangeColor(uint8 ColorIndex)
{
	if (ColorIndex < static_cast<uint8>(EMGPlayerColor::Red) || static_cast<uint8>(EMGPlayerColor::Gray) < ColorIndex)
	{
		return;
	}

	ServerRPCSetColor(static_cast<EMGPlayerColor>(ColorIndex));
}

void AMGPlayerController::ServerRPCSetColor_Implementation(EMGPlayerColor NewColor)
{
	// Server RPC라 서버에서 실행 → 게임모드 접근 가능
	AMGLobbyGameModeBase* LGM = Cast<AMGLobbyGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (IsValid(LGM))
	{
		LGM->OnPlayerChangeColor(this, NewColor);
	}
}

void AMGPlayerController::ServerRPCSetReady_Implementation(bool bReady)
{
	// Server RPC라 서버에서 실행 → 게임모드 접근 가능
	AMGLobbyGameModeBase* LGM = Cast<AMGLobbyGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (IsValid(LGM))
	{
		LGM->OnPlayerReady(this, bReady);
	}
}
