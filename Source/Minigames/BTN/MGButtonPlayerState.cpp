#include "MGButtonPlayerState.h"
#include "Net/UnrealNetwork.h"

AMGButtonPlayerState::AMGButtonPlayerState()
{
    FinalScore = 0;
    FinalRank = 0;
}

void AMGButtonPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMGButtonPlayerState, FinalScore);
    DOREPLIFETIME(AMGButtonPlayerState, FinalRank);
}

void AMGButtonPlayerState::OnRep_ResultData()
{
}