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

TArray<AMGButtonPlayerState*> AMGButtonGameState::GetSortedPlayerStates()
{
    TArray<AMGButtonPlayerState*> SortedPlayers;

    for (APlayerState* PS : PlayerArray)
    {
        if (PS == nullptr) continue;

        AMGButtonPlayerState* ButtonPS = Cast<AMGButtonPlayerState>(PS);

        // 캐스팅 실패 시, 객체 클래스 이름을 출력
        if (ButtonPS)
        {
            SortedPlayers.Add(ButtonPS);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("DEBUG: Cast FAILED. Real Class Name: %s"), *PS->GetClass()->GetName());
        }
    }

    // 소유권 개수 순으로 내림차순 정렬
    SortedPlayers.Sort([](const AMGButtonPlayerState& A, const AMGButtonPlayerState& B) {
        return A.GetScore() > B.GetScore();
        });

    return SortedPlayers;
}