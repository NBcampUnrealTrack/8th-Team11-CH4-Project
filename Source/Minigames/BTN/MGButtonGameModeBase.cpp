#include "MGButtonGameModeBase.h"
#include "MGButtonPlayerState.h"
#include "GameState/MGGameStateBase.h"
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
}

void AMGButtonGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 종료 시 실행 중인 타이머 정리
    GetWorldTimerManager().ClearTimer(GameTimerHandle);

    Super::EndPlay(EndPlayReason);
}

// 게임 시작
void AMGButtonGameModeBase::StartMinigame()
{
    Super::StartMinigame();

    CurrentPhase = EGamePhase::Playing;
    TimeRemaining = GameDuration;

    if (AMGButtonGameState* GS = GetGameState<AMGButtonGameState>())
    {
        GS->CurrentPhase = CurrentPhase;
        GS->TimeRemaining = TimeRemaining;
        GS->OnGamePhaseChanged.Broadcast(CurrentPhase);
    }

    GetWorldTimerManager().SetTimer(
        GameTimerHandle,
        this,
        &AMGButtonGameModeBase::AdvanceTimer,
        1.0f,
        true);
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

    if (TimeRemaining <= 0)
    {
        EndMinigame();
    }
}

void AMGButtonGameModeBase::EndMinigame()
{

    CurrentPhase = EGamePhase::GameOver;

    if (AMGButtonGameState* GS = GetGameState<AMGButtonGameState>())
    {
        GS->CurrentPhase = CurrentPhase;
        GS->OnGamePhaseChanged.Broadcast(CurrentPhase);
    }

    TArray<AMGButtonPlayerState*> PlayerStates;

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (AMGButtonPlayerState* PS = Cast<AMGButtonPlayerState>(PC->PlayerState))
            {
                PlayerStates.Add(PS);
            }
        }
    }

    if (PlayerStates.Num() > 0)
    {
        // 순위 정렬
        PlayerStates.Sort([](const AMGButtonPlayerState& A, const AMGButtonPlayerState& B)
            {
                return A.GetScore() > B.GetScore();
            });

        int32 CurrentRank = 1;

        // 순위 계산
        for (int32 i = 0; i < PlayerStates.Num(); ++i)
        {
            if (i > 0 && PlayerStates[i]->GetScore() < PlayerStates[i - 1]->GetScore())
            {
                CurrentRank = i + 1;
            }

            // 점수 지급
            GiveScore(PlayerStates[i], CurrentRank);
        }

        // 전체 점수 기준 정렬
        PlayerStates.Sort([](const AMGButtonPlayerState& A, const AMGButtonPlayerState& B)
            {
                return A.TotalScore > B.TotalScore;
            });

        int32 OverallRank = 1;

        // 전체 랭킹 계산
        for (int32 i = 0; i < PlayerStates.Num(); ++i)
        {
            if (i > 0 && PlayerStates[i]->TotalScore < PlayerStates[i - 1]->TotalScore)
            {
                OverallRank = i + 1;
            }

            PlayerStates[i]->Rank = OverallRank;
        }

        // 버튼 점수 초기화
        for (AMGButtonPlayerState* PS : PlayerStates)
        {
            PS->SetScore(0.f);
        }
    }

    Super::EndMinigame();
}