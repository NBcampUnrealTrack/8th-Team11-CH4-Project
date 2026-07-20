// MGPlayerCharacter.h

#pragma once

#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MGPlayerCharacter.generated.h"

// 플레이어가 폭탄넘기기 동작을 취할때 수신받는 델리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTryPassBomb, bool /*CollisionEnabledValue*/);

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;
class UMGStatusComponent;
class UMGNameWidgetComponent;
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

	FORCEINLINE FRotator GetCurrentCamRot() const { return CurrentCamRot; }

	UMGStatusComponent* GetMGStatusComponent() const { return StatusComponent; }

	bool FillCharacterColor();

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

	uint8 bFalling : 1;

#pragma endregion

#pragma region Input

private:
	void HandleMoveInput(const FInputActionValue& InValue);

	void HandleLookInput(const FInputActionValue& InValue);

	UFUNCTION(Server, Unreliable) // 한 두번 정도는 씹혀도 되기 때문.
	void ServerRPCUpdateCamRot(const FRotator& InCamRot);

	// 상호작용 액션 (깃발 뺏기 등)
	void HandleInteractionInput(const FInputActionValue& InValue);
	void HandleInteractionEndInput(const FInputActionValue& InValue);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(Replicated)
	FRotator CurrentCamRot = FRotator::ZeroRotator;

	FRotator PreviousCamRot = FRotator::ZeroRotator;

	// 깃발 뺏기 입력 액션
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MGPlayerCharacter|Input")
	TObjectPtr<UInputAction> InteractionAction;
#pragma endregion

#pragma region Gimmick
public:
	void PlayPassBombMontage(bool Value);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SetCarryState(bool Value);

	UFUNCTION(Server, Reliable)
	void ServerRPC_SendPlayPassBombRequest(bool Value);

	UFUNCTION(Client, Reliable)
	void ClientRPC_PlayPassBombMontage(AMGPlayerCharacter* TargetCharacter, bool Value);

public:
	FOnTryPassBomb OnTryPassBombDelegate;	// 폭탄넘기기를 시도할 때 실행하는 델리게이트

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> PassBombMontage;

private:
	bool IsCarrying = false;	// 규칙 핵심 아이템(폭탄, 깃발)을 들고있는가?
#pragma endregion
};
