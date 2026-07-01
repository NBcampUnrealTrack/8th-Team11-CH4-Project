#include "MGButtonGameState.h"
#include "Net/UnrealNetwork.h"

AMGButtonGameState::AMGButtonGameState() : CurrentPhase(EGamePhase::WaitingToStart), TimeRemaining(0) {}

void AMGButtonGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMGButtonGameState, CurrentPhase);
    DOREPLIFETIME(AMGButtonGameState, TimeRemaining);
}