// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MGLobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MINIGAMES_API AMGLobbyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetReady(bool bReady);
	
	UPROPERTY(ReplicatedUsing = OnRep_IsReady, VisibleAnywhere, BlueprintReadOnly)
	bool bIsReady = false;
	
private:
	UFUNCTION()
	void OnRep_IsReady();   // 클라에서 bIsReady 변경 시 UI 갱신용
};
