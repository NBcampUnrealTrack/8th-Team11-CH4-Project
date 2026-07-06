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
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	CamArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("DeathCamSpringArm"));
	CamArm->SetupAttachment(RootComponent);
	CamArm->TargetArmLength = 300.f;

	Cam = CreateDefaultSubobject<UCameraComponent>(TEXT("DeathCam"));
	Cam->SetupAttachment(CamArm);
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
		CamArm->SetWorldLocation(FollowingMesh->GetBoneLocation(MeshPelvisName));
	}
}

void AMGSpectatorPawn::DeathCamFollowCharacter(APlayerController* PC, ACharacter* Character, FName PelvisName, float FollowingTime)
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
			CamArm->SetWorldLocation(FollowingMesh->GetBoneLocation(MeshPelvisName));
		}

		FRotator DeathCamRotation;
		DeathCamRotation = PC->GetControlRotation();
		DeathCamRotation.Pitch = -90.f;
		CamArm->SetWorldRotation(DeathCamRotation);
	}

	AMGPlayerController* MGPC = Cast<AMGPlayerController>(PC);
	PC->SetViewTargetWithBlend(this, 0.5f, EViewTargetBlendFunction::VTBlend_EaseOut, 1.f);

	GetWorld()->GetTimerManager().SetTimer(
		DeathTimeHandle,
		this,
		&ThisClass::OnDeathTimerEnd,
		FollowingTime,
		false
	);
}

void AMGSpectatorPawn::SpectateOtherPlayer(int32 idx)
{
	AMGGameStateBase* GS = GetWorld()->GetGameState<AMGGameStateBase>();
	
	if (IsValid(GS))
	{
		if (GS->AlivePlayers.IsValidIndex(idx) == true)
		{
			
		}
	}
}

void AMGSpectatorPawn::OnDeathTimerEnd()
{
	SetActorTickEnabled(false);
	FollowingMesh = nullptr;
}
