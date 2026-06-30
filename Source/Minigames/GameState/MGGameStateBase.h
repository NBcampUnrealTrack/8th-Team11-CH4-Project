// MGGameStateBase.h

#pragma once

#include "GameFramework/GameStateBase.h"
#include "MGGameStateBase.generated.h"

UENUM(BlueprintType)
enum class EMatchState : uint8
{
	None,
	Entering,			 // 심리스 트래블 후 클라이언트들이 맵에 들어오길 기다리는 상태
	Waiting,			 // 모든 클라이언트가 Waiting 상태에 진입 시 Playing으로 전환하는 Timer 시작
	Playing,
	RoundResult,		 // 해당 미니게임 결과 UI 표시 (게임 조작 불가, FInputModeUIOnly)
	Ending,				 // ServerTravel을 호출하기 직전 마무리 단계
	ALL_COUNT UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ERoundState : uint8
{
	None,
	Lobby,
	Round1,
	Round2,
	Round3,
	FinalResult,			// FinalResult 상태에서 Lobby로 돌아가는 UI 표시
	ALL_COUNT UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ELobbyState : uint8
{
	None,
	Connecting,		 // 접속 중 (아직 UI 조작 불가)
	NotReady,		 // 로비에서 대기중
	Ready,           // 준비 완료 (모든 유저가 Ready면 ServerTravel 호출)
	ALL_COUNT UMETA(Hidden)
};

UCLASS()
class MINIGAMES_API AMGGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 AllPlayerControllerCount = 0;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "GameFlow")
	EMatchState MatchState = EMatchState::Waiting;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameFlow")
	ERoundState RoundState = ERoundState::None;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameFlow")
	ELobbyState LobbyState = ELobbyState::None;
};
