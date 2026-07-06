#include "MGButtonGameModeBase.h"
#include "MGButtonPlayerState.h"
#include "PlayerState/MGPlayerState.h"
#include "GameState/MGGameStateBase.h"
#include "Controller/MGPlayerController.h"
#include "GameFramework/PlayerController.h"

AMGButtonGameModeBase::AMGButtonGameModeBase()
{
    PlayerStateClass = AMGButtonPlayerState::StaticClass();

    CurrentPhase = EGamePhase::WaitingToStart;
    TimeRemaining = 0;
}

void AMGButtonGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    StartReadyPhase();
}

// 게임 준비
void AMGButtonGameModeBase::StartReadyPhase()
{
    CurrentPhase = EGamePhase::WaitingToStart;
    TimeRemaining = ReadyDuration;

    UE_LOG(LogTemp, Warning, TEXT("게임 준비 (%d초)"), TimeRemaining);

    GetWorldTimerManager().SetTimer(
        GameTimerHandle,
        this,
        &AMGButtonGameModeBase::AdvanceTimer,
        1.0f,
        true);
}

void AMGButtonGameModeBase::StartPlayingPhase()
{
    CurrentPhase = EGamePhase::Playing;
    TimeRemaining = GameDuration;

    UE_LOG(LogTemp, Warning, TEXT("게임 시작 (%d초)"), TimeRemaining);

    if (AMGButtonGameState* GS = GetGameState<AMGButtonGameState>())
    {
        GS->OnGamePhaseChanged.Broadcast(CurrentPhase);
    }
}

// 타이머 갱신 로직
void AMGButtonGameModeBase::AdvanceTimer()
{
    TimeRemaining--;

    if (AMGButtonGameState* GS = GetGameState<AMGButtonGameState>())
    {
        GS->TimeRemaining = TimeRemaining;

        if (GS->CurrentPhase != CurrentPhase)
        {
            GS->CurrentPhase = CurrentPhase;
            GS->OnGamePhaseChanged.Broadcast(CurrentPhase);
        }
    }

    if (CurrentPhase == EGamePhase::WaitingToStart)
    {
        UE_LOG(LogTemp, Log, TEXT("시작 카운트다운: %d"), TimeRemaining);

        if (TimeRemaining <= 0)
        {
            StartPlayingPhase();
        }
    }
    else if (CurrentPhase == EGamePhase::Playing)
    {
        UE_LOG(LogTemp, Warning, TEXT("남은 게임 시간: %d"), TimeRemaining);

        if (TimeRemaining <= 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("시간 종료! EndMinigame  호출"));

            GetWorldTimerManager().ClearTimer(GameTimerHandle);
            EndMinigame();
        }
    }
}

void AMGButtonGameModeBase::EndMinigame()
{
    Super::EndMinigame();

    UE_LOG(LogTemp, Warning, TEXT("EndMinigame 함수 진입"));

    TArray<AMGButtonPlayerState*> PlayerStates;

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (AMGButtonPlayerState* PS =
                Cast<AMGButtonPlayerState>(PC->PlayerState))
            {
                PlayerStates.Add(PS);
            }
        }
    }

    if (PlayerStates.Num() > 0)
    {
        // 순위 결정
        PlayerStates.Sort(
            [](const AMGButtonPlayerState& A,
                const AMGButtonPlayerState& B)
            {
                return A.GetScore() > B.GetScore();
            });

        int32 CurrentRank = 1;

        for (int32 i = 0; i < PlayerStates.Num(); ++i)
        {
            if (i > 0 &&
                PlayerStates[i]->GetScore() <
                PlayerStates[i - 1]->GetScore())
            {
                CurrentRank = i + 1;
            }

            float ScoreBeforeBonus =
                PlayerStates[i]->GetScore();

            Super::GiveScore(
                Cast<AMGPlayerState>(PlayerStates[i]),
                CurrentRank);

            float ScoreAfterBonus =
                PlayerStates[i]->GetScore();

            float BonusPoints =
                ScoreAfterBonus - ScoreBeforeBonus;

            UE_LOG(LogTemp, Log, TEXT("플레이어: %s | 소유권 점수: %f | 등수: %d등 | 획득한 등수 보너스: %f"),
                *PlayerStates[i]->GetPlayerName(),
                ScoreBeforeBonus,
                CurrentRank,
                BonusPoints);
        }
    }

    if (AMGGameStateBase* MGGameState =
        GetGameState<AMGGameStateBase>())
    {
        MGGameState->MatchState = EMatchState::Ending;
    }
}