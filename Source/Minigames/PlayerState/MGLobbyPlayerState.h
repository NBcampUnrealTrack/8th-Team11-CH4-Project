// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Type/MGPlayerColor.h"
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
	
	UPROPERTY(ReplicatedUsing = OnRep_PlayerColor, VisibleAnywhere, BlueprintReadOnly)
	EMGPlayerColor PlayerColor;
	
protected:
	virtual void OnRep_PlayerName() override;
	
private:
	UFUNCTION()
	void OnRep_IsReady();

	UFUNCTION()
	void OnRep_PlayerColor();
};
