#include "MGButtonGameState.h"
#include "MGButtonPlayerState.h" 
#include "Net/UnrealNetwork.h"

AMGButtonGameState::AMGButtonGameState()
{
    bReplicates = true;
    CurrentPhase = EGamePhase::WaitingToStart;
    TimeRemaining = 0;
}

void AMGButtonGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMGButtonGameState, CurrentPhase);
    DOREPLIFETIME(AMGButtonGameState, TimeRemaining);
}

void AMGButtonGameState::OnRep_CurrentPhase()
{
    OnGamePhaseChanged.Broadcast(CurrentPhase);
}

void AMGButtonGameState::OnRep_TimeRemaining() {}

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

    SortedPlayers.Sort([](const AMGButtonPlayerState& A, const AMGButtonPlayerState& B)
        {
            return A.GetScore() > B.GetScore();
        });

    return SortedPlayers;
}

TArray<AMGButtonPlayerState*> AMGButtonGameState::GetSortedPlayerStatesByTotalScore()
{
    TArray<AMGButtonPlayerState*> SortedPlayers;

    for (APlayerState* PS : PlayerArray)
    {
        if (AMGButtonPlayerState* ButtonPS = Cast<AMGButtonPlayerState>(PS))
        {
            SortedPlayers.Add(ButtonPS);
        }
    }

    SortedPlayers.Sort([](const AMGButtonPlayerState& A, const AMGButtonPlayerState& B)
        {
            return A.Rank < B.Rank;
        });

    return SortedPlayers;
}