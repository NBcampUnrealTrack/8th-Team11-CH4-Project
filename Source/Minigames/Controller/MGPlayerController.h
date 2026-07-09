// MGPlayerController.h

#pragma once

#include "GameFramework/PlayerController.h"
#include "MGPlayerController.generated.h"

enum class EMGPlayerColor : uint8;
class UUserWidget;
class UUW_GameResult;
class UUW_LobbyLayout;
class ULevelSequence;

class UMGChat;
/**
 *
 */
UCLASS()
class MINIGAMES_API AMGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnCharacterDead();

	UFUNCTION(Client, Reliable)
	void ClientRPCShowGameResultWidget(int32 InRanking);

	UFUNCTION(Client, Reliable)
	void ClientRPCReturnToTitle();
	
	UFUNCTION(Server, Reliable)
	void ServerRPCSetReady(bool bReady);

	// [임시 테스트용] 콘솔에서 "ChangeColor 0"~"ChangeColor 9" 입력 → 색 변경 검증용. 나중에 색상 버튼 UI로 대체
	UFUNCTION(Exec)
	void ChangeColor(uint8 ColorIndex);

	// [정식] 클라 → 서버로 색 변경 요청.
	UFUNCTION(Server, Reliable)
	void ServerRPCSetColor(EMGPlayerColor NewColor);

	// [정식] 클라 → 서버로 해당 폰 빙의 요청. Possess는 서버에서 실행되어야함.
	UFUNCTION(Server, Reliable)
	void ServerRPCPossess(APawn* InPawn);
	
	UFUNCTION(Server, Reliable)
	void ServerRPCSetNickname(const FString& InNickname);
	
public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FText NotificationText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> NotificationTextUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUW_GameResult> GameResultUIClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUW_LobbyLayout> LobbyLayoutClass;

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

	UFUNCTION(Client,Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION()
	void CreateChatWidget();

	UFUNCTION(Client, Reliable)
	void ClientRPCOnSeamlessTravelCompleted();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMGChat>ChatWidgetClass;

	UPROPERTY()
	TObjectPtr<UMGChat>ChatWidgetInstance;

	FString ChatMessageString;

#pragma endregion
};
