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
	void DeathCamFollowCharacter(ACharacter* Character, FName PelvisName);

	void SpectateOtherPlayer(int32 idx);

protected:
	UFUNCTION()
	void OnDeathTimerEnd();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class UCameraComponent> Cam;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class USpringArmComponent> CamArm;

	UPROPERTY()
	USkeletalMeshComponent* FollowingMesh;

	FName MeshPelvisName;

	FTimerHandle DeathTimeHandle;
};
