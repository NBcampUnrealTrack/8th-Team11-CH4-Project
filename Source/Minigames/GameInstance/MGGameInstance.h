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
	// 레벨 이동 시 저장해야할 데이터 임시로 보관
public:
	
};
