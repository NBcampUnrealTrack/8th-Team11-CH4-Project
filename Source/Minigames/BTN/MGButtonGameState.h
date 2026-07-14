#pragma once

#include "CoreMinimal.h"
#include "GameState/MGGameStateBase.h"
#include "MGButtonPlayerState.h"
#include "MGButtonGameState.generated.h"

class AMGPlayerState;

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
    WaitingToStart, // 준비
    Countdown,      // 카운트다운
    Playing,        // 게임 플레이
    GameOver        // 게임 종료 및 집계
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChanged, EGamePhase, NewPhase);

UCLASS()
class MINIGAMES_API AMGButtonGameState : public AMGGameStateBase
{
    GENERATED_BODY()

public:
    AMGButtonGameState();

    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnGamePhaseChanged OnGamePhaseChanged;

    UFUNCTION() void OnRep_CurrentPhase();
    UFUNCTION() void OnRep_TimeRemaining();

    UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase, BlueprintReadOnly, Category = "Game")
    EGamePhase CurrentPhase;

    UPROPERTY(ReplicatedUsing = OnRep_TimeRemaining, BlueprintReadOnly, Category = "Game")
    int32 TimeRemaining;

    // 소유권 개수 순으로 플레이어 정렬
    UFUNCTION(BlueprintCallable, Category = "Scoreboard")
    TArray<AMGButtonPlayerState*> GetSortedPlayerStatesByScore();

    // 토탈 점수 순으로 플레이어 정렬
    UFUNCTION(BlueprintCallable, Category = "Scoreboard")
    TArray<AMGButtonPlayerState*> GetSortedPlayerStatesByTotalScore();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};