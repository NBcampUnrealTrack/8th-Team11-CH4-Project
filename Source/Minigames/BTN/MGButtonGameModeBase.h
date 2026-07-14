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

protected:
    virtual void BeginPlay() override;

    // 게임 진행 함수
    virtual void StartMinigame() override;
    void AdvanceTimer();
    virtual void EndMinigame() override;

    // 타이머
    FTimerHandle GameTimerHandle;

    UPROPERTY(EditAnywhere, Category = "Game Settings")
    int32 GameDuration = 30;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
    int32 TimeRemaining;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
    EGamePhase CurrentPhase;
};