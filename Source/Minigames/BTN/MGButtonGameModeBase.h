#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MGButtonGameModeBase.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
    WaitingToStart, // 준비 
    Countdown,      // 카운트다운
    Playing,        // 게임 플레이 
    GameOver        // 게임 종료 및 집계
};

UCLASS()
class MINIGAMES_API AMGButtonGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    AMGButtonGameModeBase();

protected:
    virtual void BeginPlay() override;

    // 게임 진행 단계 함수
    void StartReadyPhase();
    void StartPlayingPhase();
    void AdvanceTimer();
    void EndGamePhase();
    void CalculateFinalScores();

    // 상태 및 타이머 변수
    UPROPERTY(BlueprintReadOnly, Category = "Game")
    EGamePhase CurrentPhase = EGamePhase::WaitingToStart; 

    FTimerHandle GameTimerHandle;

    UPROPERTY(BlueprintReadOnly, Category = "Game")
    int32 TimeRemaining;

    // 시간 설정
    UPROPERTY(EditAnywhere, Category = "Game Settings")
    int32 ReadyDuration = 5; // 게임 준비 시간

    UPROPERTY(EditAnywhere, Category = "Game Settings")
    int32 GameDuration = 30; // 게임 진행 시간
};