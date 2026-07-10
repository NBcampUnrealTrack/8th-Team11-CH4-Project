#include "MGButtonPlayerState.h"
#include "Net/UnrealNetwork.h"

AMGButtonPlayerState::AMGButtonPlayerState()
{
}

void AMGButtonPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
