// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MGSpectatorPawn.h"
#include "Controller/MGPlayerController.h"
#include "GameState/MGGameStateBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"

#include "Minigames.h"

AMGSpectatorPawn::AMGSpectatorPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	CamArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("DeathCamSpringArm"));
	CamArm->SetupAttachment(RootComponent);
	CamArm->TargetArmLength = 300.f;

	Cam = CreateDefaultSubobject<UCameraComponent>(TEXT("DeathCam"));
	Cam->SetupAttachment(CamArm);

	FollowingMesh = nullptr;
}

void AMGSpectatorPawn::BeginPlay()
{
	Super::BeginPlay();
	CamArm->SetAbsolute(false, true, false);
}

void AMGSpectatorPawn::DeathCamFollowCharacter(ACharacter* Character)
{
	const float CamBlendTime = 0.5;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	MG_LOG_NET(LogMGNet, Log, TEXT("SpectatorPawn: %s"), *GetName());

	if (IsValid(Character))
	{
		FollowingMesh = Character->GetMesh();
		MG_LOG_NET(LogMGNet, Log, TEXT("FollowingMesh: %s"), *FollowingMesh->GetName());

		if (IsValid(FollowingMesh))
		{
			CamArm->AttachToComponent(FollowingMesh, FAttachmentTransformRules::KeepWorldTransform);
			CamArm->SetRelativeLocation(FollowingMesh->GetRelativeLocation() * -1.f);
		}

		FRotator DeathCamRotation;
		DeathCamRotation = PC->GetControlRotation();
		DeathCamRotation.Pitch = -90.f;
		CamArm->SetWorldRotation(DeathCamRotation);
	}

	AMGPlayerController* MGPC = Cast<AMGPlayerController>(PC);
	PC->SetViewTargetWithBlend(this, CamBlendTime, EViewTargetBlendFunction::VTBlend_EaseOut, 1.f);

	SetTimerToChangeTarget();
}

void AMGSpectatorPawn::SpectateOtherPlayer(int32 idx)
{
	AMGGameStateBase* GS = GetWorld()->GetGameState<AMGGameStateBase>();
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (IsValid(GS))
	{
		if (GS->AliveCharacters.IsValidIndex(idx) == true)
		{
			ACharacter* Character = GS->AliveCharacters[idx];
			if (IsValid(Character))
			{
				FollowingMesh = Character->GetMesh();
				CamArm->AttachToComponent(FollowingMesh, FAttachmentTransformRules::KeepWorldTransform);
				CamArm->SetRelativeLocation(FollowingMesh->GetRelativeLocation() * -1.f);
				CamArm->SetRelativeRotation(FRotator::ZeroRotator);
				PC->SetViewTarget(this);
			}
		}
	}
}

void AMGSpectatorPawn::SetTimerToChangeTarget()
{
	const float FollowingTime = 3.f;

	MG_LOG_NET(LogMGNet, Log, TEXT("has Called."));

	GetWorld()->GetTimerManager().ClearTimer(DeathTimeHandle);
	GetWorld()->GetTimerManager().SetTimer(
		DeathTimeHandle,
		this,
		&ThisClass::OnDeathTimerEnd,
		FollowingTime,
		false
	);
}

void AMGSpectatorPawn::OnDeathTimerEnd()
{
	//GetWorld()->GetTimerManager().ClearTimer(DeathTimeHandle);

	FollowingMesh = nullptr;
	AMGGameStateBase* MGGS = GetWorld()->GetGameState<AMGGameStateBase>();
	if (IsValid(MGGS))
	{
		int32 idx = FMath::RandRange(0, MGGS->AliveCharacters.Num() - 1);
		SpectateOtherPlayer(idx);
	}
}
