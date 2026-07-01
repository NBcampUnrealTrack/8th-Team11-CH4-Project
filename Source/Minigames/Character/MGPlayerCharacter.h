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
class UMGHPTextWidgetComponent;
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

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Components")
	TObjectPtr<UMGStatusComponent> StatusComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Components")
	TObjectPtr<UMGHPTextWidgetComponent> HPTextWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMGInteractionOverlapComponent> InteractionComponent;

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
	void SetHPTextWidget(UUW_HPText* InHPTextWidget);

	void TakeBuff(float InBuffValue);

#pragma endregion

#pragma region RuleComponent
public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SetRule(TSubclassOf<UActorComponent> RuleComp);

	FORCEINLINE UActorComponent* GetRule() { return CurrentRule; }
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Rule")
	TObjectPtr<UActorComponent> CurrentRule;

#pragma endregion


#pragma region FlagState

	//맵이 넘어가면 깃발 뺏기 게임이 끝나므로 계속 값을 가지고 있을 필요 없을 것 같아 캐릭터 재생성시 제거되도록 여기 구현
public:

	bool SetHasFlag(bool bHasFlag);

	FORCEINLINE bool GetHasFlag() const
	{
		return bFlagState;
	}

private:

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCTakeFlag();

private:

	UPROPERTY(Replicated)
	bool bFlagState = false;

#pragma endregion
};
