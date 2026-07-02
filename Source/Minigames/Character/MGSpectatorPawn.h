// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "MGSpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class MINIGAMES_API AMGSpectatorPawn : public APawn
{
	GENERATED_BODY()
public:
	AMGSpectatorPawn();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void DeathCamFollowCharacter(APlayerController* PC, ACharacter* Character, float time);

protected:
	UFUNCTION()
	void OnDeathTimerEnd();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class UCameraComponent> DeathCam;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class USpringArmComponent> DeathCamArm;

	UPROPERTY()
	USkeletalMeshComponent* FollowingMesh;

	FTimerHandle DeathTimeHandle;
};
