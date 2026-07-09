// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "MGSpectatorPawn.generated.h"

/**
 * 
 */
class UInputMappingContext;
class UInputAction;
class AMGPlayerController;
UCLASS()
class MINIGAMES_API AMGSpectatorPawn : public APawn
{
	GENERATED_BODY()
public:
	AMGSpectatorPawn();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void DeathCamFollowCharacter(ACharacter* Character);

	void SpectateOtherPlayer(int32 idx);

	FORCEINLINE const USkeletalMeshComponent* GetFollowingMesh() { return FollowingMesh; }

	void SetTimerToChangeTarget();	// 본인 탈락, 관전대상 탈락 등에 호출되는 관전자 교체타이머 함수

protected:
	UFUNCTION()
	void OnDeathTimerEnd();

	virtual void OnRep_Owner() override;
	
private:
	void HandleLookInput(const FInputActionValue& InValue);

	void HandleSpectateInput(const FInputActionValue& InValue);

protected:
	UPROPERTY()
	TObjectPtr<AMGPlayerController> OwnerPC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class UCameraComponent> Cam;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class USpringArmComponent> CamArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SpectateAction;

	UPROPERTY()
	ACharacter* FollowingCharacter;

	UPROPERTY()
	USkeletalMeshComponent* FollowingMesh;

	FTimerHandle DeathTimeHandle;
};
