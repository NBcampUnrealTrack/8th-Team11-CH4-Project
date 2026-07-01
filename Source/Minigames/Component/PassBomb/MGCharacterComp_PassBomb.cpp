// Fill out your copyright notice in the Description page of Project Settings.


#include "MGCharacterComp_PassBomb.h"

#include "Minigames.h"

// Sets default values for this component's properties
UMGCharacterComp_PassBomb::UMGCharacterComp_PassBomb()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UMGCharacterComp_PassBomb::BeginPlay()
{
	Super::BeginPlay();

	MG_LOG_NET(LogMGNet, Log, TEXT("Owner: %s"), *GetOwner()->GetName());
}

void UMGCharacterComp_PassBomb::MulticastRPC_RetireCharacter_Implementation()
{
	MG_LOG_NET(LogMGNet, Log, TEXT("%s is Retired"), *GetOwner()->GetName());
}
