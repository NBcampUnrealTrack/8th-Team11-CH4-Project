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
	DeathCamArm->TargetArmLength = 500.f;

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
		//DeathCamArm->SetWorldLocation(FollowingMesh->GetBoneLocation(FollowingMesh->GetBoneName(0)));
	}
}

void AMGSpectatorPawn::DeathCamFollowCharacter(APlayerController* PC, ACharacter* Character, float time)
{

	SetActorTickEnabled(true);
	FollowingMesh = Character->GetMesh();

	MG_LOG_NET(LogMGNet, Log, TEXT("FollowingMesh: %s"), *FollowingMesh->GetName());
	MG_LOG_NET(LogMGNet, Log, TEXT("FollowingMeshBone: %s"), *FollowingMesh->GetBoneName(0).ToString());
	MG_LOG_NET(LogMGNet, Log, TEXT("FollowingMeshBoneLoc: %s"), *FollowingMesh->GetBoneLocation(FollowingMesh->GetBoneName(0)).ToString());
	//DeathCamArm->SetWorldLocation(FollowingMesh->GetBoneLocation(FollowingMesh->GetBoneName(0)));
	DeathCamArm->SetWorldLocation(FVector(1000.f,1000.f,1000.f));

	FRotator DeathCamRotation;
	DeathCamRotation = Character->GetActorRotation();
	DeathCamRotation.Pitch = -90.f;
	DeathCamArm->SetWorldRotation(DeathCamRotation);

	//PC->SetViewTargetWithBlend(this, 0.5f, EViewTargetBlendFunction::VTBlend_EaseOut, 1.f);
	
	GetWorld()->GetTimerManager().SetTimer(
		DeathTimeHandle,
		this,
		&ThisClass::OnDeathTimerEnd,
		time,
		false
	);
}

void AMGSpectatorPawn::OnDeathTimerEnd()
{
	SetActorTickEnabled(false);
	FollowingMesh = nullptr;
}
