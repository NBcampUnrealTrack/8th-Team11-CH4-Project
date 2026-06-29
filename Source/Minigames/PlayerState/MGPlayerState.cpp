// MGPlayerState.cpp


#include "PlayerState/MGPlayerState.h"
#include "Net/UnrealNetwork.h"

void AMGPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMGPlayerState, Rank);
}
