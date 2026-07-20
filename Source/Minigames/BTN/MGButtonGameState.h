#pragma once

#include "CoreMinimal.h"
#include "GameState/MGGameStateBase.h"
#include "MGButtonGameState.generated.h"

class AMGButtonPlayerState;

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
    WaitingToStart,
    Countdown,
    Playing,
    GameOver
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChanged, EGamePhase, NewPhase);

UCLASS()
class MINIGAMES_API AMGButtonGameState : public AMGGameStateBase
{
    GENERATED_BODY()

public:

    AMGButtonGameState();

    // 게임 페이즈 변경
    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnGamePhaseChanged OnGamePhaseChanged;


    UFUNCTION()
    void OnRep_CurrentPhase();

    UFUNCTION()
    void OnRep_TimeRemaining();

    // 현재 페이즈
    UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase, BlueprintReadOnly, Category = "Game")
    EGamePhase CurrentPhase;

    // 게임 진행 시간
    UPROPERTY(ReplicatedUsing = OnRep_TimeRemaining, BlueprintReadOnly, Category = "Game")
    int32 TimeRemaining;

    // 소유권 개수 순 정렬
    UFUNCTION(BlueprintCallable, Category = "Scoreboard")
    TArray<AMGButtonPlayerState*> GetSortedPlayerStatesByScore();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;
};