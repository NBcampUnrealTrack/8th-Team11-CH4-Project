// MGGameStateBase.cpp


#include "GameState/MGGameStateBase.h"

#include "Net/UnrealNetwork.h"

void AMGGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, AllPlayerControllerCount);
	DOREPLIFETIME(ThisClass, MatchState);
	DOREPLIFETIME(ThisClass, RoundState);
	DOREPLIFETIME(ThisClass, LobbyState);
}
