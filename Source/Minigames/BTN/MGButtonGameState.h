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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChanged, EGamePhase, NewPhase);

UCLASS()
class MINIGAMES_API AMGButtonGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    AMGButtonGameState();

    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnGamePhaseChanged OnGamePhaseChanged;

    UFUNCTION()
    void OnRep_CurrentPhase();

    UFUNCTION()
    void OnRep_TimeRemaining();

    UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase, BlueprintReadOnly, Category = "Game")
    EGamePhase CurrentPhase;

    UPROPERTY(ReplicatedUsing = OnRep_TimeRemaining, BlueprintReadOnly, Category = "Game")
    int32 TimeRemaining;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};