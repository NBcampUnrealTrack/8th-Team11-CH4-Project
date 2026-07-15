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
#include "UI/UW_FinalResult.h"

#include "LevelSequence.h"						// Level Sequence
#include "LevelSequencePlayer.h"				// Level Sequence
#include "MovieSceneSequencePlayer.h"			// Level Sequence
#include "GameFramework/PlayerState.h"
#include "GameInstance/MGGameInstance.h"
#include "GameMode/MGFinalResultGameModeBase.h"

#include "UI/Chat/MGChat.h"
#include "EngineUtils.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "UI/Chat/MGChatPopup.h"
#include "UI/Chat/MGChatPopupList.h"

#include "Data/MGMinigameInfoRow.h"
#include "UI/Loading/UW_MinigameIntro.h"
#include "Engine/DataTable.h"

void AMGPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController() == false)
	{
		return;
	}

	if (UMGGameInstance* GI = GetGameInstance<UMGGameInstance>())
	{
		if (GI->PlayerNickname.IsEmpty() == false)
		{
			ServerRPCSetNickname(GI->PlayerNickname);
		}
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

	// (일반 트래블: PC가 재생성되어 BeginPlay 경로로 생성됨.
	//  심리스 트래블: PC가 유지되어 BeginPlay가 다시 불리지 않으므로
	//  ClientRPCOnSeamlessTravelCompleted 경로로 재생성 — 두 경로 모두 필요함)
		CreateChatWidget();
	}
	else
	{
		// 미니게임
		FInputModeGameOnly GameOnly;
		SetInputMode(GameOnly);
		bShowMouseCursor = false;
		
		CreateChatWidget();
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

void AMGPlayerController::ShowMinigameIntro()
{
	if (IsLocalController() == false || MinigameIntroClass == nullptr)
	{
		return;
	}

	UMGGameInstance* GI = GetGameInstance<UMGGameInstance>();
	if (IsValid(GI) == false || IsValid(GI->MinigameInfoTable) == false)
	{
		return;
	}

	// 현재 맵(=목적지) 이름으로 행 조회. 행 없으면(로비 등) 그냥 안 띄움 → 자연스러운 가드
	const FName RowName(*GI->PendingDestinationMapName);
	FMGMinigameInfoRow* Row = GI->MinigameInfoTable->FindRow<FMGMinigameInfoRow>(RowName, TEXT("ShowMinigameIntro"));
	if (Row == nullptr)
	{
		return;
	}

	if (IsValid(MinigameIntroInstance) == false)
	{
		MinigameIntroInstance = CreateWidget<UUW_MinigameIntro>(this, MinigameIntroClass);
	}

	if (IsValid(MinigameIntroInstance))
	{
		MinigameIntroInstance->Setup(*Row);
		MinigameIntroInstance->AddToViewport();
	}
}

void AMGPlayerController::HideMinigameIntro()
{
	if (IsValid(MinigameIntroInstance))
	{
		MinigameIntroInstance->RemoveFromParent();
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
	if (IsLocalController() == false)
	{
		return;
	}
	if (IsValid(GameResultUIClass) == false)
	{
		return;
	}
	
	UUW_GameResult* GameResultUI = CreateWidget<UUW_GameResult>(this, GameResultUIClass);
	if (IsValid(GameResultUI) == true)
	{
		GameResultUI->AddToViewport(3);

		FString GameResultString = FString::Printf(TEXT("%s"), InRanking == 1 ? TEXT("Winner Winner!") : TEXT("Loser..."));
		GameResultUI->ResultText->SetText(FText::FromString(GameResultString));

		FString RankingString = FString::Printf(TEXT("#%02d"), InRanking);
		GameResultUI->RankingText->SetText(FText::FromString(RankingString));

		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(GameResultUI->GetCachedWidget());
		SetInputMode(Mode);

		bShowMouseCursor = true;
	}
}

void AMGPlayerController::ClientRPC_SetResultCamera_Implementation()
{
	ResultCameraRetryCount = 0;
	TrySetResultCamera();
}

void AMGPlayerController::TrySetResultCamera()
{
	TArray<AActor*> Cams;
	UGameplayStatics::GetAllActorsWithTag(this, TEXT("ResultCamera"), Cams);
	if (Cams.Num() > 0 && IsValid(Cams[0]))
	{
		bAutoManageActiveCameraTarget = false;
		SetViewTargetWithBlend(Cams[0], 0.5f);
		return;
	}

	// 카메라 액터가 아직 스폰 안 됨(레벨 로딩 중) → 재시도 (0.2초 * 20 = 최대 4초)
	if (ResultCameraRetryCount < 20)
	{
		++ResultCameraRetryCount;
		GetWorldTimerManager().SetTimer(
			ResultCameraRetryHandle,
			this,
			&ThisClass::TrySetResultCamera,
			0.2f,
			false
		);
	}
}

void AMGPlayerController::ClientRPC_ShowFinalResult_Implementation()
{
	if (IsLocalController() == false)
	{
		return;
	}
	if (IsValid(FinalResultWidget))
	{
		return;
	}
	if (IsValid(FinalResultWidgetClass) == false)
	{
		return;
	}
	
	FinalResultWidget = CreateWidget<UUW_FinalResult>(this, FinalResultWidgetClass);
	if (IsValid(FinalResultWidget) == false)
	{
		return;
	}
	
	FinalResultWidget->AddToViewport(3);
	
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void AMGPlayerController::ChangeColor(uint8 ColorIndex)
{
	if (ColorIndex < static_cast<uint8>(EMGPlayerColor::Red) || static_cast<uint8>(EMGPlayerColor::Gray) < ColorIndex)
	{
		return;
	}

	ServerRPCSetColor(static_cast<EMGPlayerColor>(ColorIndex));
}

void AMGPlayerController::ServerRPC_ReadyToReturn_Implementation()
{
	if (AMGFinalResultGameModeBase* FGM = GetWorld()->GetAuthGameMode<AMGFinalResultGameModeBase>())
	{
		FGM->OnPlayerReadyToReturn(this);
	}
}

void AMGPlayerController::ServerRPCSetNickname_Implementation(const FString& InNickname)
{
	if (InNickname.IsEmpty() == true)
	{
		return;
	}
	
	if (APlayerState* PS = GetPlayerState<APlayerState>())
	{
		PS->SetPlayerName(InNickname);
	}
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
		HideAllWidgets();
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

	RestoreAllWidgets();
}

#pragma endregion

#pragma region Chat
void AMGPlayerController::CreateChatWidget()
{
	if (IsLocalController() == false)
	{
		return;
	}
	if (IsValid(ChatWidgetInstance) == true)
	{
		ChatWidgetInstance->RemoveFromParent();
		ChatWidgetInstance = nullptr;
	}
	if (IsValid(ChatWidgetClass) == true)
	{
		ChatWidgetInstance = CreateWidget<UMGChat>(this, ChatWidgetClass);
		if (IsValid(ChatWidgetInstance) == true)
		{
			ChatWidgetInstance->AddToViewport(10);

			bool bVisible = false;

			UMGGameInstance* MGGameInstance = GetGameInstance<UMGGameInstance>();
			if (IsValid(MGGameInstance) == true)
			{
				bVisible = MGGameInstance->bChatVisible;

				for (const FMGChatType& Message : MGGameInstance->ChatMessageHistory)
				{
					ChatWidgetInstance->AddChatMessage(Message);
				}
			}

			if (bVisible == true)
			{
				ChatWidgetInstance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else
			{
				ChatWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
	if (IsValid(ChatPopupListInstance) == true)
	{
		ChatPopupListInstance->RemoveFromParent();
		ChatPopupListInstance = nullptr;
	}
	if (IsValid(ChatPopupListClass) == true)
	{
		ChatPopupListInstance = CreateWidget<UMGChatPopupList>(this, ChatPopupListClass);
		if (IsValid(ChatPopupListInstance) == true)
		{
			ChatPopupListInstance->AddToViewport(9); 
			ChatPopupListInstance->SetVisibility(ESlateVisibility::HitTestInvisible); 
		}
	}
}

void AMGPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	if (IsLocalController() == true)
	{
		ServerRPCPrintChatMessageString(InChatMessageString);
	}
}

void AMGPlayerController::ClientRPCPrintChatMessage_Implementation(const FMGChatType& InChatMessageString)
{
	UMGGameInstance* MGGameInstance = GetGameInstance<UMGGameInstance>();
	if (IsValid(MGGameInstance) == true)
	{
		MGGameInstance->ChatMessageHistory.Add(InChatMessageString);
		while (MGGameInstance->ChatMessageHistory.Num() > MaxChatHistory)
		{
			MGGameInstance->ChatMessageHistory.RemoveAt(0);
		}
	}

	if (IsValid(ChatWidgetInstance) == true)
	{
		ChatWidgetInstance->AddChatMessage(InChatMessageString);

		bool bIsMyMessage = false;
		if (IsValid(PlayerState) == true)
		{
			bIsMyMessage = (InChatMessageString.SenderName == PlayerState->GetPlayerName());
		}
		if (bIsMyMessage == false
			&& ChatWidgetInstance->GetVisibility() == ESlateVisibility::Collapsed
			&& IsValid(ChatPopupListInstance) == true)
		{
			ChatPopupListInstance->AddPopup(InChatMessageString);
		}
	}
}
bool AMGPlayerController::ServerRPCPrintChatMessageString_Validate(const FString& InChatMessageString)
{
	return InChatMessageString.Len() <= MaxChatLength;
}


void AMGPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	FMGChatType ChatMessage;
	ChatMessage.Message = InChatMessageString;

	if (IsValid(PlayerState) == true)
	{
		ChatMessage.SenderName = PlayerState->GetPlayerName();

		UMGGameInstance* GI = GetGameInstance<UMGGameInstance>();
		if (IsValid(GI) == true)
		{
			if (const FMGPlayerSaveData* FoundData = GI->SavedPlayerData.Find(PlayerState->GetUniqueId()))
			{
				ChatMessage.SenderColor = FoundData->Color;
			}
		}
	}

	for (TActorIterator<AMGPlayerController> It(GetWorld()); It; ++It)
	{
		AMGPlayerController* MGPlayerController = *It;
		if (IsValid(MGPlayerController) == true)
		{
			MGPlayerController->ClientRPCPrintChatMessage(ChatMessage);
		}
	}
}

void AMGPlayerController::ClientRPCOnSeamlessTravelCompleted_Implementation()
{
	CreateChatWidget();
	ShowMinigameIntro();
}

void AMGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	FInputKeyBinding& ToggleChatBinding =
		InputComponent->BindKey(EKeys::Enter, IE_Pressed, this, &AMGPlayerController::OnEnterKeyPressed);
	ToggleChatBinding.bConsumeInput = false;
}

void AMGPlayerController::OnEnterKeyPressed()
{
	if (IsValid(ChatWidgetInstance) == false)
	{
		return;
	}

	if (ChatWidgetInstance->GetVisibility() == ESlateVisibility::Collapsed)
	{
		ChatWidgetInstance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (IsValid(ChatPopupListInstance) == true)
		{
			ChatPopupListInstance->ClearAllPopups();
		}
		if (UMGGameInstance* GI = GetGameInstance<UMGGameInstance>())
		{
			GI->bChatVisible = true;
		}
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	ChatWidgetInstance->FocusChatInput();
}

void AMGPlayerController::RestoreDefaultInputMode()
{
	if (GetWorld()->GetGameState<AMGLobbyGameStateBase>() != nullptr)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
	else
	{
		FInputModeGameOnly GameOnly;
		SetInputMode(GameOnly);
		bShowMouseCursor = false;
	}

	UWidgetBlueprintLibrary::SetFocusToGameViewport();
}

#pragma endregion

void AMGPlayerController::HideAllWidgets()
{
	SavedWidgetVisibilities.Empty();

	TArray<UUserWidget*> AllWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), AllWidgets, UUserWidget::StaticClass(), true);

	for (UUserWidget* Widget : AllWidgets)
	{
		if (IsValid(Widget) == true)
		{
			SavedWidgetVisibilities.Add(Widget, Widget->GetVisibility());
			Widget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void AMGPlayerController::RestoreAllWidgets()
{
	for (const auto& Pair : SavedWidgetVisibilities)
	{
		if (Pair.Key.IsValid() == true)
		{
			Pair.Key->SetVisibility(Pair.Value);
		}
	}
	SavedWidgetVisibilities.Empty();
}

void AMGPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(ResultCameraRetryHandle);

	Super::EndPlay(EndPlayReason);
}
