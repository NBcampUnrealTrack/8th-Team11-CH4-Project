// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MGSpectatorPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"

#include "Minigames.h"

AMGSpectatorPawn::AMGSpectatorPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	DeathCamArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("DeathCamSpringArm"));
	DeathCamArm->SetupAttachment(RootComponent);
	DeathCamArm->TargetArmLength = 300.f;

	DeathCam = CreateDefaultSubobject<UCameraComponent>(TEXT("DeathCam"));
	DeathCam->SetupAttachment(DeathCamArm);
}

void AMGSpectatorPawn::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}

void AMGSpectatorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsValid(FollowingMesh))
	{
		DeathCamArm->SetWorldLocation(FollowingMesh->GetBoneLocation(MeshPelvisName));
	}
}

void AMGSpectatorPawn::DeathCamFollowCharacter(APlayerController* PC, ACharacter* Character, FName PelvisName, float Following)
{
	if (PC->IsLocalController() == false)
	{
		return;
	}
	MG_LOG_NET(LogMGNet, Log, TEXT("SpectatorPawn: %s"), *GetName());

	SetActorTickEnabled(true);
	if (IsValid(Character))
	{
		FollowingMesh = Character->GetMesh();
		MG_LOG_NET(LogMGNet, Log, TEXT("FollowingMesh: %s"), *FollowingMesh->GetName());

		if (FollowingMesh->GetBoneIndex(PelvisName) != INDEX_NONE)
		{
			MeshPelvisName = PelvisName;
			MG_LOG_NET(LogMGNet, Log, TEXT("FollowingMeshBone: %s"), *MeshPelvisName.ToString());
			MG_LOG_NET(LogMGNet, Log, TEXT("FollowingMeshBone_InitialLoc: %s"), *FollowingMesh->GetBoneLocation(MeshPelvisName).ToString());
			DeathCamArm->SetWorldLocation(FollowingMesh->GetBoneLocation(MeshPelvisName));
		}

		FRotator DeathCamRotation;
		DeathCamRotation = PC->GetControlRotation();
		DeathCamRotation.Pitch = -90.f;
		DeathCamArm->SetWorldRotation(DeathCamRotation);
	}

	PC->SetViewTargetWithBlend(this, 0.5f, EViewTargetBlendFunction::VTBlend_EaseOut, 1.f);

	//GetWorld()->GetTimerManager().SetTimer(
	//	DeathTimeHandle,
	//	this,
	//	&ThisClass::OnDeathTimerEnd,
	//	time,
	//	false
	//);
}

void AMGSpectatorPawn::OnDeathTimerEnd()
{
	SetActorTickEnabled(false);
	FollowingMesh = nullptr;
}
