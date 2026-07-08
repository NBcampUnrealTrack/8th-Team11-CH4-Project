#include "MGButtonPlayerState.h"
#include "Net/UnrealNetwork.h"

AMGButtonPlayerState::AMGButtonPlayerState()
{
    bReplicates = true;
}

// 블루프린트 순위, 점수 동기화
void AMGButtonPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMGButtonPlayerState, FinalRank);
    DOREPLIFETIME(AMGButtonPlayerState, FinalScore);
}