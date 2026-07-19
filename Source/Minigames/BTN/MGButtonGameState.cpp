#include "MGButtonGameState.h"
#include "MGButtonPlayerState.h"
#include "Net/UnrealNetwork.h"

AMGButtonGameState::AMGButtonGameState()
{
    bReplicates = true;

    CurrentPhase = EGamePhase::WaitingToStart;
    TimeRemaining = 0;
}

void AMGButtonGameState::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMGButtonGameState, CurrentPhase);
    DOREPLIFETIME(AMGButtonGameState, TimeRemaining);
}

// 게임 페이즈 변경
void AMGButtonGameState::OnRep_CurrentPhase()
{
    OnGamePhaseChanged.Broadcast(CurrentPhase);
}

// TODO: TimeRemaining Replication 수신 시 타이머 UI 업데이트 처리
// 현재는 Blueprint에서 값을 직접 참조
void AMGButtonGameState::OnRep_TimeRemaining()
{
}

// 소유권 개수 순 정렬
TArray<AMGButtonPlayerState*> AMGButtonGameState::GetSortedPlayerStatesByScore()
{
    TArray<AMGButtonPlayerState*> SortedPlayers;

    for (APlayerState* PS : PlayerArray)
    {
        if (AMGButtonPlayerState* ButtonPS = Cast<AMGButtonPlayerState>(PS))
        {
            SortedPlayers.Add(ButtonPS);
        }
    }

    SortedPlayers.Sort([](
        const AMGButtonPlayerState& A,
        const AMGButtonPlayerState& B)
        {
            return A.GetScore() > B.GetScore();
        });

    return SortedPlayers;
}

