// MGPlayerController.h

#pragma once

#include "GameFramework/PlayerController.h"
#include "Type/MGChatType.h"
#include "MGPlayerController.generated.h"

enum class EMGPlayerColor : uint8;
class UUserWidget;
class UUW_GameResult;
class UUW_LobbyLayout;
class ULevelSequence;
class UUW_FinalResult;
class UMGChat;
class UUW_MinigameIntro;
class UUW_FinalResult;


UCLASS()
class MINIGAMES_API AMGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(Client, Reliable)
	void ClientRPCShowGameResultWidget();

	UFUNCTION(Client, Reliable)
	void ClientRPCReturnToTitle();
	
	UFUNCTION(Client, Reliable)
	void ClientRPC_SetResultCamera();

	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowFinalResult();
	
	UFUNCTION(Server, Reliable)
	void ServerRPCSetReady(bool bReady);

	// 클라 → 서버로 색 변경 요청.
	UFUNCTION(Server, Reliable)
	void ServerRPCSetColor(EMGPlayerColor NewColor);

	void ClearInputMapping();

	// [정식] 클라 → 서버로 해당 폰 빙의 요청. Possess는 서버에서 실행되어야함.
	UFUNCTION(Server, Reliable)
	void ServerRPCPossess(APawn* InPawn);
	
	UFUNCTION(Server, Reliable)
	void ServerRPCSetNickname(const FString& InNickname);
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_ReadyToReturn();
	
	void OnCharacterDead();

	void ShowMinigameIntro();

	void HideMinigameIntro();

	UFUNCTION()
	void SetupMinigameEnv();

protected:
	void TryBindGameStateDelegate();
	
public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FText NotificationText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> NotificationTextUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUW_GameResult> GameResultUIClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUW_LobbyLayout> LobbyLayoutClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUW_FinalResult> FinalResultWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Intro")
	TSubclassOf<UUW_MinigameIntro> MinigameIntroClass;

	UPROPERTY()
	TObjectPtr<UUW_MinigameIntro> MinigameIntroInstance;
	
private:
	void TrySetResultCamera();

	UPROPERTY()
	TObjectPtr<UUW_FinalResult> FinalResultWidget;
	
	int32 ResultCameraRetryCount = 0;
	
	FTimerHandle ResultCameraRetryHandle;

	FTimerHandle DelegateBindTimerHandler;

#pragma region CutScene

public:
	// ClientRPC, 서버에서 모든 Client들에게 해당 함수를 실행하라고 명령
	UFUNCTION(Client, Reliable)
	void ClientRPC_PlayCutScene(int32 MGCutSceneIndex);

	UFUNCTION()
	void OnCutSceneFinished();

	const TArray<TObjectPtr<ULevelSequence>>& GetCutSceneAssets() const { return CutSceneAssets; }

protected:
	// 블루프린트에서 만든 Level Sequence 에셋 포인터를 저장할 TArray
	UPROPERTY(EditDefaultsOnly, Category = "Cinematic")
	TArray<TObjectPtr<ULevelSequence>> CutSceneAssets;

#pragma endregion


#pragma region Chat

public:
	void SetChatMessageString(const FString& InChatMessgeString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessage(const FMGChatType& InChatMessage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION()
	void CreateChatWidget();

	UFUNCTION(Client, Reliable)
	void ClientRPCOnSeamlessTravelCompleted();

	void RestoreDefaultInputMode();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMGChat>ChatWidgetClass;

	UPROPERTY()
	TObjectPtr<UMGChat>ChatWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Chat")
	TSubclassOf<class UMGChatPopupList> ChatPopupListClass;

	UPROPERTY()
	TObjectPtr<class UMGChatPopupList> ChatPopupListInstance;

	FString ChatMessageString;

	virtual void SetupInputComponent() override;
	void OnEnterKeyPressed();

	static constexpr int32 MaxChatLength = 200;

	static constexpr int32 MaxChatHistory = 100;

#pragma endregion

public:
	void HideAllWidgets();
	void RestoreAllWidgets();

protected:
	TMap<TWeakObjectPtr<UUserWidget>, ESlateVisibility> SavedWidgetVisibilities;
};
