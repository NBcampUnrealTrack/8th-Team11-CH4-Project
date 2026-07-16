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

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsCarrying : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bomb")
	uint8 bPassBomb : 1;

protected:
	UPROPERTY()
	TObjectPtr<AMGPlayerCharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	uint8 bShouldMove : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	uint8 bIsFalling : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Head")
	FRotator CurrentHeadRot = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Head")
	FRotator TargetHeadRot = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Head")
	uint8 bHeadDirection : 1;
};
