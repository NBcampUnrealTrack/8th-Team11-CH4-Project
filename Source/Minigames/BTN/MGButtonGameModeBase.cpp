#include "MGButtonGameModeBase.h"
#include "PlayerState/MGFlagPlayerState.h"
#include "GameState/MGGameStateBase.h"
#include "Controller/MGPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

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
        UE_LOG(LogTemp, Warning, TEXT("남은 게임 시간: %d"), TimeRemaining); 

        if (TimeRemaining <= 0)
        {
            UE_LOG(LogTemp, Error, TEXT("시간 종료! EndGamePhase 호출"));
            GetWorldTimerManager().ClearTimer(GameTimerHandle);
            EndGamePhase();
        }
    }
}

void AMGButtonGameModeBase::EndGamePhase()
{
    UE_LOG(LogTemp, Warning, TEXT("!!! EndGamePhase 함수 진입 !!!"));

    TArray<AMGFlagPlayerState*> PlayerStates;

    for (AMGPlayerController* PC : AllPlayerControllers)
    {
        if (IsValid(PC) && PC->PlayerState)
        {
            if (AMGFlagPlayerState* PS = Cast<AMGFlagPlayerState>(PC->PlayerState))
            {
                PlayerStates.Add(PS);
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("수집된 플레이어 수: %d"), PlayerStates.Num());

    if (PlayerStates.Num() > 0)
    {
        // 순위 정렬
        PlayerStates.Sort([](const AMGFlagPlayerState& A, const AMGFlagPlayerState& B) {
            return A.MGScore > B.MGScore;
            });

        // 점수 부여
        int32 CurrentRank = 1;
        for (int32 i = 0; i < PlayerStates.Num(); ++i)
        {
            if (i > 0 && PlayerStates[i]->MGScore < PlayerStates[i - 1]->MGScore)
            {
                CurrentRank = i + 1;
            }
            GiveScore(PlayerStates[i], CurrentRank);
        }
    }

    // 3게임 상태 변경
    if (AMGGameStateBase* MGGameState = GetGameState<AMGGameStateBase>())
    {
        MGGameState->MatchState = EMatchState::Ending;
    }
}

// 점수 부여 함수
void AMGButtonGameModeBase::GiveScore(AMGFlagPlayerState* PS, int32 Rank)
{
    // 부모 클래스의 점수 부여 로직을 그대로 사용
    if (AMGGameModeBase* ParentGM = Cast<AMGGameModeBase>(GetWorld()->GetAuthGameMode()))
    {
        ParentGM->GiveScore(PS, Rank);
    }
}

// 점수 집계 테스트
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

