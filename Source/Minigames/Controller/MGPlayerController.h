// MGPlayerController.h

#pragma once

#include "GameFramework/PlayerController.h"
#include "MGPlayerController.generated.h"

class UUserWidget;
class UUW_GameResult;

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
	
	// [임시 테스트용] 콘솔에서 "Ready" / "Unready" 입력 → 로직 검증용. 나중에 버튼으로 대체
	UFUNCTION(Exec)
	void Ready();

	UFUNCTION(Exec)
	void Unready();
	
	// [정식] 클라 → 서버로 레디 전달. 나중에 레디 버튼도 이걸 호출
	UFUNCTION(Server, Reliable)
	void ServerRPCSetReady(bool bReady);

public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FText NotificationText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> NotificationTextUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUW_GameResult> GameResultUIClass;

};
