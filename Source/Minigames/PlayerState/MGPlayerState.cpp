// MGPlayerState.cpp


#include "PlayerState/MGPlayerState.h"
#include "Net/UnrealNetwork.h"

#include "Net/UnrealNetwork.h"

AMGPlayerState::AMGPlayerState()
	: TotalScore(0)
{
	bReplicates = true;
}

void AMGPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TotalScore);
}
