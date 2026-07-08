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

	UFUNCTION(BlueprintCallable)
	void DeathCamFollowCharacter(ACharacter* Character);

	void SpectateOtherPlayer(int32 idx);

	FORCEINLINE const USkeletalMeshComponent* GetFollowingMesh() { return FollowingMesh; }

	void SetTimerToChangeTarget();	// 본인 탈락, 관전대상 탈락 등에 호출되는 관전자 교체타이머 함수

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

	UPROPERTY()
	FTimerHandle DeathTimeHandle;
};
