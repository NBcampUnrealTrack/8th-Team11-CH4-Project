#include "MGButtonGameModeBase.h"
#include "MGButtonPlayerState.h"
#include "GameState/MGGameStateBase.h"
#include "GameFramework/PlayerController.h"

AMGButtonGameModeBase::AMGButtonGameModeBase()
{
    PrimaryActorTick.bCanEverTick = true;

    PlayerStateClass = AMGButtonPlayerState::StaticClass();

    CurrentPhase = EGamePhase::WaitingToStart;
    TimeRemaining = 0;

    bGameEnded = false;

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

    if (CurrentPhase == EGamePhase::Playing)
    {
        return;
    }

    // 이전 종료 상태 초기화
    bGameEnded = false;

    // 기존 타이머 제거
    GetWorldTimerManager().ClearTimer(GameTimerHandle);

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

// 타이머 갱신
void AMGButtonGameModeBase::AdvanceTimer()
{
    if (bGameEnded)
    {
        return;
    }

    if (TimeRemaining <= 0)
    {
        GetWorldTimerManager().ClearTimer(GameTimerHandle);
        EndMinigame();
        return;
    }

    TimeRemaining--;

    if (AMGButtonGameState* GS = GetGameState<AMGButtonGameState>())
    {
        GS->TimeRemaining = FMath::Max(TimeRemaining, 0);

        if (GS->CurrentPhase != CurrentPhase)
        {
            GS->CurrentPhase = CurrentPhase;
            GS->OnGamePhaseChanged.Broadcast(CurrentPhase);
        }
    }

    if (TimeRemaining <= 0)
    {
        TimeRemaining = 0;

        GetWorldTimerManager().ClearTimer(GameTimerHandle);

        EndMinigame();

        return;
    }
}

void AMGButtonGameModeBase::EndMinigame()
{
    if (bGameEnded || CurrentPhase == EGamePhase::GameOver)
    {
        return;
    }

    bGameEnded = true;

    GetWorldTimerManager().ClearTimer(GameTimerHandle);

    CurrentPhase = EGamePhase::GameOver;

    TArray<AMGButtonPlayerState*> PlayerStates;


    // GameState PlayerArray 기준으로 가져오기
    if (AMGButtonGameState* GS = GetGameState<AMGButtonGameState>())
    {
        for (APlayerState* BasePS : GS->PlayerArray)
        {
            if (AMGButtonPlayerState* PS = Cast<AMGButtonPlayerState>(BasePS))
            {
                PlayerStates.Add(PS);
            }
        }
    }


    if (PlayerStates.Num() > 0)
    {
        // 현재 미니게임 점수 기준 정렬
        PlayerStates.Sort([](
            const AMGButtonPlayerState& A,
            const AMGButtonPlayerState& B)
            {
                return A.GetScore() > B.GetScore();
            });


        int32 CurrentRank = 1;

        for (int32 i = 0; i < PlayerStates.Num(); ++i)
        {
            if (i > 0 &&
                PlayerStates[i]->GetScore() < PlayerStates[i - 1]->GetScore())
            {
                CurrentRank = i + 1;
            }

            GiveScore(PlayerStates[i], CurrentRank);
        }


        // 누적 점수 기준 정렬
        PlayerStates.Sort([](
            const AMGButtonPlayerState& A,
            const AMGButtonPlayerState& B)
            {
                return A.TotalScore > B.TotalScore;
            });


        int32 OverallRank = 1;

        for (int32 i = 0; i < PlayerStates.Num(); ++i)
        {
            if (i > 0 &&
                PlayerStates[i]->TotalScore < PlayerStates[i - 1]->TotalScore)
            {
                OverallRank = i + 1;
            }

            PlayerStates[i]->Rank = OverallRank;
        }


        // 서버 실제 값 확인
        for (AMGButtonPlayerState* PS : PlayerStates)
        {
            UE_LOG(LogTemp, Warning, TEXT("Result PlayerState Exists"));
        }


        /*
        // 테스트 중이므로 제거
        for (AMGButtonPlayerState* PS : PlayerStates)
        {
            PS->SetScore(0.f);
        }
        */
    }


    Super::EndMinigame();


    if (AMGButtonGameState* GS = GetGameState<AMGButtonGameState>())
    {
        GS->CurrentPhase = CurrentPhase;
        GS->TimeRemaining = 0;
        GS->EndingTimeRemaining = RemainWaitingTimeForEnding;
        GS->OnGamePhaseChanged.Broadcast(CurrentPhase);
    }
}

void AMGButtonGameModeBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (CurrentPhase != EGamePhase::GameOver)
    {
        return;
    }

    AMGButtonGameState* GS = GetGameState<AMGButtonGameState>();
    if (!GS)
    {
        return;
    }

    GS->EndingTimeRemaining = RemainWaitingTimeForEnding;
}