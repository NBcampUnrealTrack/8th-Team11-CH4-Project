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

#include "LevelSequence.h"						// Level Sequence
#include "LevelSequencePlayer.h"				// Level Sequence
#include "MovieSceneSequencePlayer.h"			// Level Sequence


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

void AMGPlayerController::ServerRPCPossess_Implementation(APawn* InPawn)
{
	if (InPawn != nullptr)
	{
		Possess(InPawn);
	}
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

#pragma region CutScene

// ClientRPC에선 _Implementation을 붙이기
void AMGPlayerController::ClientRPC_PlayCutScene_Implementation(int32 MGCutSceneIndex)
{
	if (!CutSceneAssets.IsValidIndex(MGCutSceneIndex))
	{
		return;
	}	// 컷신 index가 유효한지 검사

	ULevelSequence* TargetSequence = CutSceneAssets[MGCutSceneIndex];
	if (!IsValid(TargetSequence))
	{
		return;
	}	// 해당 컷신이 실제로 있는지 검사

	ALevelSequenceActor* OutActor;
	ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(),
		TargetSequence,
		FMovieSceneSequencePlaybackSettings(),
		OutActor
	);

	if (IsValid(SequencePlayer))
	{
		// Client 제어권 뺏기
		// 시네마틱 모드 ON: 이동 불가, 마우스 회전 불가, UI 숨김
		SetCinematicMode(true, false, false, true, true);

		// 컷신이 끝나면 OnCutSceneFinished 함수가 자동으로 실행되도록 델리게이트 바인딩
		SequencePlayer->OnFinished.AddDynamic(this, &AMGPlayerController::OnCutSceneFinished);

		// 컷신 재생
		SequencePlayer->Play();
	}
}

void AMGPlayerController::OnCutSceneFinished()
{
	// 컷신이 끝나면 다시 Client에게 제어권을 돌려줌 
	// 시네마틱 모드 OFF
	SetCinematicMode(false, false, false, true, true);

	// 카메라도 원래 Client 각자의 캐릭터 시점으로 안전하게 복귀
	SetViewTarget(GetPawn());
}

#pragma endregion
