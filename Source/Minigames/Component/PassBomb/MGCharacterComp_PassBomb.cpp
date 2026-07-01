// Fill out your copyright notice in the Description page of Project Settings.


#include "MGCharacterComp_PassBomb.h"

#include "Minigames.h"

// Sets default values for this component's properties
UMGCharacterComp_PassBomb::UMGCharacterComp_PassBomb()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMGCharacterComp_PassBomb::BeginPlay()
{
	Super::BeginPlay();

	MG_LOG_NET(LogMGNet, Log, TEXT("Owner: %s"), *GetOwner()->GetName());
}

void UMGCharacterComp_PassBomb::MulticastRPC_RetireCharacter_Implementation()
{
	MG_LOG_NET(LogMGNet, Log, TEXT("%s has retired."), *GetOwner()->GetName());
}


// Called every frame
void UMGCharacterComp_PassBomb::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

