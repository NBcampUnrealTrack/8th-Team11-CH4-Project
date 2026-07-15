// MGAnimInstanceBase.h

#pragma once

#include "Animation/AnimInstance.h"
#include "MGAnimInstanceBase.generated.h"

class AMGPlayerCharacter;
class UCharacterMovementComponent;

/**
 *
 */
UCLASS()
class MINIGAMES_API UMGAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UFUNCTION()
	void AnimNotify_CheckMeleeAttackHit();

protected:
	UPROPERTY()
	TObjectPtr<AMGPlayerCharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bShouldMove : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsFalling : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator CurrentHeadRot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator TargetHeadRot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool HeadDirection;
};
