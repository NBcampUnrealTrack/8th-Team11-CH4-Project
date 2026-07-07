// MGPlayerController.h

#pragma once

#include "GameFramework/PlayerController.h"
#include "MGPlayerController.generated.h"

enum class EMGPlayerColor : uint8;
class UUserWidget;
class UUW_GameResult;
class UUW_LobbyLayout;

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
	
public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FText NotificationText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> NotificationTextUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUW_GameResult> GameResultUIClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUW_LobbyLayout> LobbyLayoutClass;
};
