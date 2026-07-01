#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MGButtonGameState.h" 
#include "MGButtonGameModeBase.generated.h"

UCLASS()
class MINIGAMES_API AMGButtonGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    AMGButtonGameModeBase();

protected:
    virtual void BeginPlay() override;

    // 게임 진행 함수 
    void StartReadyPhase();
    void StartPlayingPhase();
    void AdvanceTimer();
    void EndGamePhase();
    void CalculateFinalScores();

    // 타이머  
    FTimerHandle GameTimerHandle;

    UPROPERTY(EditAnywhere, Category = "Game Settings")
    int32 ReadyDuration = 10; // 게임 준비 시간

    UPROPERTY(EditAnywhere, Category = "Game Settings")
    int32 GameDuration = 30; // 게임 진행 시간

    // 현재 상태와 남은 시간 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
    int32 TimeRemaining;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
    EGamePhase CurrentPhase;
};