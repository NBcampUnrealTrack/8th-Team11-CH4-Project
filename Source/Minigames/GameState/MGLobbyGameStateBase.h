// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MGLobbyGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class MINIGAMES_API AMGLobbyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentPlayerCount = 0;
	
	// 남은 카운트다운 시간 (10→0). 0이면 카운트다운 미진행
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 RemainCountdownTime = 0;
};
