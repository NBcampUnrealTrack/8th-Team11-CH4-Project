// // MGFlagGameStateBase.cpp


#include "GameState/MGFlagGameStateBase.h"

#include "Net/UnrealNetwork.h"

void AMGFlagGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, RemainGameTime);

}
