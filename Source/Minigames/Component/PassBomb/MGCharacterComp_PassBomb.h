// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MGCharacterComp_PassBomb.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINIGAMES_API UMGCharacterComp_PassBomb : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMGCharacterComp_PassBomb();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_RetireCharacter();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
