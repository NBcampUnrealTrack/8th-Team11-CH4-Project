#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MGButtonGameState.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
    WaitingToStart, // 준비
    Countdown,      // 카운트다운
    Playing,        // 게임 플레이
    GameOver        // 게임 종료 및 집계
};

UCLASS()
class MINIGAMES_API AMGButtonGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    AMGButtonGameState();

    // 클라이언트 상태 및 타이머 변수 
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    EGamePhase CurrentPhase;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    int32 TimeRemaining;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};