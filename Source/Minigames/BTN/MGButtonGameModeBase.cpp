#include "MGButtonGameModeBase.h"
#include "MGButtonGameState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

AMGButtonGameModeBase::AMGButtonGameModeBase()
{
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

    GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AMGButtonGameModeBase::AdvanceTimer, 1.0f, true);
}

// 게임 시작
void AMGButtonGameModeBase::StartPlayingPhase()
{
    CurrentPhase = EGamePhase::Playing;
    TimeRemaining = GameDuration;

    UE_LOG(LogTemp, Warning, TEXT("게임 시작 (%d초) "), TimeRemaining);
}

// 타이머 갱신 로직
void AMGButtonGameModeBase::AdvanceTimer()
{
    TimeRemaining--;

    // GameState로 데이터 전달
    if (AMGButtonGameState* GS = GetGameState<AMGButtonGameState>())
    {
        GS->TimeRemaining = TimeRemaining;
        GS->CurrentPhase = CurrentPhase;
    }

    if (CurrentPhase == EGamePhase::WaitingToStart)
    {
        UE_LOG(LogTemp, Log, TEXT("시작 카운트다운: %d"), TimeRemaining);
        if (TimeRemaining <= 0) StartPlayingPhase();
    }
    else if (CurrentPhase == EGamePhase::Playing)
    {
        UE_LOG(LogTemp, Log, TEXT("남은 게임 시간: %d"), TimeRemaining);
        if (TimeRemaining <= 0)
        {
            GetWorldTimerManager().ClearTimer(GameTimerHandle);
            EndGamePhase();
        }
    }
}

// 게임 종료
void AMGButtonGameModeBase::EndGamePhase()
{
    CurrentPhase = EGamePhase::GameOver;
    UE_LOG(LogTemp, Warning, TEXT("게임 종료!"));
    CalculateFinalScores();
}

// 점수 집계
void AMGButtonGameModeBase::CalculateFinalScores()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (PC && PC->PlayerState)
        {
            UE_LOG(LogTemp, Warning, TEXT("플레이어: %s, 최종 버튼 개수: %f"), *PC->GetName(), PC->PlayerState->GetScore());
        }
    }
}