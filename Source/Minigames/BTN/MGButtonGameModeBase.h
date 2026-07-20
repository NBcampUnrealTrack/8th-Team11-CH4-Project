#pragma once

#include "CoreMinimal.h"
#include "GameMode/MGGameModeBase.h"
#include "MGButtonGameState.h"
#include "MGButtonGameModeBase.generated.h"

UCLASS()
class MINIGAMES_API AMGButtonGameModeBase : public AMGGameModeBase
{
    GENERATED_BODY()

public:
    AMGButtonGameModeBase();

    virtual void Tick(float DeltaSeconds) override;

    bool bGameEnded = false;

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // 게임 진행 함수
    virtual void StartMinigame() override;
    void AdvanceTimer();
    virtual void EndMinigame() override;

    // 타이머
    FTimerHandle GameTimerHandle;

    UPROPERTY(EditAnywhere, Category = "Game Settings")
    int32 GameDuration = 90;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
    int32 TimeRemaining;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
    EGamePhase CurrentPhase;
};