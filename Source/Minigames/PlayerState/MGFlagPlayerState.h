// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerState/MGPlayerState.h"
#include "MGFlagPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MINIGAMES_API AMGFlagPlayerState : public AMGPlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	float HoldingTime = 0.f;
};
