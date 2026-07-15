// MGPlayerCharacter.h

#pragma once

#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MGPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;
class UMGStatusComponent;
class UMGNameWidgetComponent;
class UUW_HPText;
class UMGInteractionOverlapComponent;

UCLASS()
class MINIGAMES_API AMGPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region MINIGAMES Override

public:
	AMGPlayerCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

#pragma endregion

#pragma region MGPlayerCharacter Components

public:
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }

	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

	float GetCurrentAimPitch() const { return CurrentAimPitch; }

	UMGStatusComponent* GetMGStatusComponent() const { return StatusComponent; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Components")
	TObjectPtr<UMGStatusComponent> StatusComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Components")
	TObjectPtr<UMGNameWidgetComponent> NameWidgetComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMGInteractionOverlapComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MGPlayerCharacter|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMGFlagActorComponent> FlagActorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MGPlayerCharacter|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> FlagMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MGPlayerCharacter|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> FlagEffectMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MGPlayerCharacter|Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNiagaraComponent> FlagNiagaraComponent;

#pragma endregion

#pragma region Input

private:
	void HandleMoveInput(const FInputActionValue& InValue);

	void HandleLookInput(const FInputActionValue& InValue);

	void HandleLandMineInput(const FInputActionValue& InValue);

	UFUNCTION(Server, Unreliable) // 한 두번 정도는 씹혀도 되기 때문.
	void ServerRPCUpdateAimValue(const float& InAimPitchValue);

	void HandleMeleeAttackInput(const FInputActionValue& InValue);

	// 깃발 뺏기 액션
	void HandleTakeFlagInput(const FInputActionValue& InValue);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Input")
	TObjectPtr<UInputAction> LandMineAction;

	UPROPERTY(Replicated)
	float CurrentAimPitch = 0.f;

	float PreviousAimPitch = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Input")
	TObjectPtr<UInputAction> MeleeAttackAction;

	// 깃발 뺏기 입력 액션
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Input")
	TObjectPtr<UInputAction> TakeFlagAction;

#pragma endregion

#pragma region LandMine

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCSpawnLandMine();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> LandMineClass;

#pragma endregion

#pragma region Attack

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void CheckMeleeAttackHit();

	UFUNCTION()
	void OnDeath();

private:
	void DrawDebugMeleeAttack(const FColor& DrawColor, FVector TraceStart, FVector TraceEnd, FVector Forward);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCMeleeAttack(float InStartMeleeAttackTime);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCMeleeAttack();

	UFUNCTION()
	void OnRep_CanAttack();

	void PlayMeleeAttackMontage();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCPerformMeleeHit(ACharacter* InDamagedCharacters, float InCheckTime);

	UFUNCTION(Client, Unreliable)
	void ClientRPCPlayMeleeAttackMontage(AMGPlayerCharacter* InTargetCharacter);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CanAttack)
	uint8 bCanAttack : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> MeleeAttackMontage;

	float MeleeAttackMontagePlayTime;

	float LastStartMeleeAttackTime;

	float MeleeAttackTimeDifference;

	float MinAllowedTimeForMeleeAttack;

#pragma endregion

#pragma region HPWidget

public:
	void TakeBuff(float InBuffValue);

#pragma endregion

};
