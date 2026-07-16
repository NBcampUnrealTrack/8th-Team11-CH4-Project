// MGAnimInstanceBase.cpp


#include "Animation/MGAnimInstanceBase.h"

#include "Character/MGPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMGAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<AMGPlayerCharacter>(GetOwningActor());
	bHeadDirection = true;
	bPassBomb = false;
	bIsCarrying = false;
	if (IsValid(OwnerCharacter) == true)
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void UMGAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!ensure(IsValid(OwnerCharacter) && IsValid(OwnerCharacterMovementComponent)))
	{
		return;
	}

	Velocity = OwnerCharacterMovementComponent->Velocity;
	GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.f).Size();
	bShouldMove = ((OwnerCharacterMovementComponent->GetCurrentAcceleration().IsNearlyZero()) == false) && (3.f < GroundSpeed);
	bIsFalling = OwnerCharacterMovementComponent->IsFalling();

	// 현재 카메라와 캐릭터의 회전값 차이
	FRotator DeltaHeadRot = OwnerCharacter->GetCurrentCamRot() - OwnerCharacter->GetActorRotation();
	DeltaHeadRot.Normalize();

	const float MaxTargetYaw = 150.f;	// 머리의 방향이 바뀌기 위한 Yaw각도
	const float MaxHeadYaw = 80.f;		// 머리의 최대Yaw
	const float MaxHeadPitch = 60.f;	// 머리의 최대Pitch
	const float HeadInterpSpeed = 10.f;	// 머리 회전속도`

	if ((bHeadDirection == true && DeltaHeadRot.Yaw >= -MaxTargetYaw) ||
		(bHeadDirection == false && DeltaHeadRot.Yaw <= MaxTargetYaw))
	{
		TargetHeadRot.Yaw = FMath::ClampAngle(DeltaHeadRot.Yaw, -MaxHeadYaw, MaxHeadYaw);
		TargetHeadRot.Pitch = FMath::ClampAngle(DeltaHeadRot.Pitch, -MaxHeadPitch, MaxHeadPitch);
		TargetHeadRot.Roll = 0.f;

		bHeadDirection = (TargetHeadRot.Yaw >= 0.f);
		CurrentHeadRot = FMath::RInterpTo(CurrentHeadRot, TargetHeadRot, DeltaSeconds, HeadInterpSpeed);
	}
}