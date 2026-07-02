#include "MGButtonGameState.h"
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

void AMGButtonGameState::OnRep_CurrentPhase() {}
void AMGButtonGameState::OnRep_TimeRemaining() {}