// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MGGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MINIGAMES_API UMGGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;

public:
	
};
