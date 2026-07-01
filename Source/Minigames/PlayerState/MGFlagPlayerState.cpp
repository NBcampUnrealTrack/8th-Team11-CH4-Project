// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/MGFlagPlayerState.h"
#include "Net/UnrealNetwork.h"


void AMGFlagPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMGFlagPlayerState, HoldingTime);
}
