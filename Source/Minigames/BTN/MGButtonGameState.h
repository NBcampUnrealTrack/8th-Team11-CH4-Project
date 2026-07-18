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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndingTimeChanged, int32, RemainingTime);

UCLASS()
class MINIGAMES_API AMGButtonGameState : public AMGGameStateBase
{
    GENERATED_BODY()

public:

    AMGButtonGameState();

    // 게임 페이즈 변경
    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnGamePhaseChanged OnGamePhaseChanged;

    // 종료 카운트 변경
    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnEndingTimeChanged OnEndingTimeChanged;

    UFUNCTION()
    void OnRep_CurrentPhase();

    UFUNCTION()
    void OnRep_TimeRemaining();

    UFUNCTION()
    void OnRep_EndingTimeRemaining();

    // 현재 페이즈
    UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase, BlueprintReadOnly, Category = "Game")
    EGamePhase CurrentPhase;

    // 게임 진행 시간
    UPROPERTY(ReplicatedUsing = OnRep_TimeRemaining, BlueprintReadOnly, Category = "Game")
    int32 TimeRemaining;

    // 게임 종료 후 카운트다운
    UPROPERTY(ReplicatedUsing = OnRep_EndingTimeRemaining, BlueprintReadOnly, Category = "Game")
    int32 EndingTimeRemaining = 15;

    // 소유권 개수 순 정렬
    UFUNCTION(BlueprintCallable, Category = "Scoreboard")
    TArray<AMGButtonPlayerState*> GetSortedPlayerStatesByScore();

    // 최종 점수 순 정렬
    UFUNCTION(BlueprintCallable, Category = "Scoreboard")
    TArray<AMGButtonPlayerState*> GetSortedPlayerStatesByTotalScore();


    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;
};