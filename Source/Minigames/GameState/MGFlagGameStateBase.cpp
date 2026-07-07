// // MGFlagGameStateBase.cpp


#include "GameState/MGFlagGameStateBase.h"
#include "Net/UnrealNetwork.h"

void AMGFlagGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMGFlagGameStateBase, RemainGameTime);
	DOREPLIFETIME(AMGFlagGameStateBase, CurrentFlagHolder);
}

void AMGFlagGameStateBase::SetCurrentFlagHolder(AMGFlagPlayerState* NewHolder)
{
	CurrentFlagHolder = NewHolder;
	OnRep_CurrentFlagHolder(); // 리슨,로컬 대비
}

void AMGFlagGameStateBase::OnRep_RemainGameTime()
{
	OnRemainTimeChanged.Broadcast(RemainGameTime);
}

void AMGFlagGameStateBase::OnRep_CurrentFlagHolder()
{
	OnFlagHolderChanged.Broadcast(CurrentFlagHolder);
}

