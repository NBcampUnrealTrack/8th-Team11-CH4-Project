// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MGSpectatorPawn.h"
#include "Controller/MGPlayerController.h"
#include "GameState/MGGameStateBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "Minigames.h"

AMGSpectatorPawn::AMGSpectatorPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicateMovement(false);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	CamArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("DeathCamSpringArm"));
	CamArm->SetupAttachment(RootComponent);
	CamArm->TargetArmLength = 300.f;

	Cam = CreateDefaultSubobject<UCameraComponent>(TEXT("DeathCam"));
	Cam->SetupAttachment(CamArm);

	FollowingCharacter = nullptr;
	FollowingMesh = nullptr;
}

void AMGSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (LookAction != nullptr)
	{
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLookInput);
	}
	if (SpectateAction != nullptr)
	{
		EIC->BindAction(SpectateAction, ETriggerEvent::Triggered, this, &ThisClass::HandleSpectateInput);
	}
}

void AMGSpectatorPawn::BeginPlay()
{
	Super::BeginPlay();
	MG_LOG_NET(LogMGNet, Log, TEXT(""));

	CamArm->SetAbsolute(false, true, false);
	OwnerPC = Cast<AMGPlayerController>(GetWorld()->GetFirstPlayerController());
}

void AMGSpectatorPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(DeathTimeHandle);

	Super::EndPlay(EndPlayReason);
}

void AMGSpectatorPawn::DeathCamFollowCharacter(ACharacter* Character)
{
	const float CamBlendTime = 0.5;

	if (HasAuthority() == true)
	{
		return;
	}
	MG_LOG_NET(LogMGNet, Log, TEXT("SpectatorPawn: %s"), *GetName());

	if (IsValid(Character))
	{
		FollowingCharacter = Character;
		FollowingMesh = FollowingCharacter->GetMesh();
		MG_LOG_NET(LogMGNet, Log, TEXT("FollowingMesh: %s"), *FollowingMesh->GetName());

		if (IsValid(FollowingMesh))
		{
			CamArm->AttachToComponent(FollowingMesh, FAttachmentTransformRules::KeepWorldTransform);
			CamArm->SetRelativeLocation(FollowingMesh->GetRelativeLocation() * -1.f);
		}

		FRotator DeathCamRotation;
		DeathCamRotation = OwnerPC->GetControlRotation();
		DeathCamRotation.Pitch = -90.f;
		CamArm->SetWorldRotation(DeathCamRotation);
	}

	OwnerPC->SetViewTargetWithBlend(this, CamBlendTime, EViewTargetBlendFunction::VTBlend_EaseOut, 1.f);

	SetTimerToChangeTarget();
}

void AMGSpectatorPawn::SpectateOtherPlayer(int32 idx)
{
	AMGGameStateBase* GS = GetWorld()->GetGameState<AMGGameStateBase>();
	if (IsValid(GS) && IsValid(OwnerPC))
	{
		if (GS->AliveCharacters.IsValidIndex(idx) == true)
		{
			ACharacter* Character = GS->AliveCharacters[idx];
			if (IsValid(Character))
			{
				FollowingCharacter = Character;
				FollowingMesh = FollowingCharacter->GetMesh();
				CamArm->AttachToComponent(FollowingMesh, FAttachmentTransformRules::KeepWorldTransform);
				CamArm->SetRelativeLocation(FollowingMesh->GetRelativeLocation() * -1.f);
				CamArm->SetRelativeRotation(FRotator::ZeroRotator);
				OwnerPC->SetViewTarget(this);
			}
		}
		if (GetOwner() == nullptr)
		{
			OwnerPC->ServerRPCPossess(this);
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
	FollowingCharacter = nullptr;
	FollowingMesh = nullptr;
	AMGGameStateBase* MGGS = GetWorld()->GetGameState<AMGGameStateBase>();
	if (IsValid(MGGS))
	{
		int32 idx = FMath::RandRange(0, MGGS->AliveCharacters.Num() - 1);
		SpectateOtherPlayer(idx);
	}
}

void AMGSpectatorPawn::OnRep_Owner()
{
	if (IsValid(OwnerPC) && GetOwner() != nullptr)
	{
		CamArm->bUsePawnControlRotation = true;

		UEnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwnerPC->GetLocalPlayer());
		if (IsValid(EILPS))
		{
			EILPS->AddMappingContext(InputMappingContext, 0);
		}
		else
		{
			MG_LOG_NET(LogMGNet, Error, TEXT("EnhancedInputLocalPlayerSubsystem is invalid."));
		}
	}
	else
	{
		MG_LOG_NET(LogMGNet, Error, TEXT("PlayerController is invalid."));
	}
}

void AMGSpectatorPawn::HandleLookInput(const FInputActionValue& InValue)
{
	if (GetOwner() == nullptr)
	{
		MG_LOG_NET(LogTemp, Error, TEXT("Controller is invalid."));
		return;
	}

	const FVector2D InLookVector = InValue.Get<FVector2D>();

	AddControllerYawInput(InLookVector.X);
	AddControllerPitchInput(InLookVector.Y);
}

void AMGSpectatorPawn::HandleSpectateInput(const FInputActionValue& InValue)
{
	MG_LOG_NET(LogMGNet, Log, TEXT("Input: %f"), InValue.Get<float>());
	if (GetOwner() == nullptr)
	{
		MG_LOG_NET(LogTemp, Error, TEXT("Controller is invalid."));
		return;
	}

	AMGGameStateBase* MGGS = GetWorld()->GetGameState<AMGGameStateBase>();
	if (IsValid(MGGS))
	{
		int32 idx = MGGS->AliveCharacters.Find(FollowingCharacter);

		if (idx == INDEX_NONE)
		{
			idx = FMath::RandRange(0, MGGS->AliveCharacters.Num() - 1);
		}
		else
		{
			idx += (InValue.Get<float>() >= 0 ? 1 : -1);
			if (idx < 0)
			{
				idx = MGGS->AliveCharacters.Num() - 1;
			}
			else if (idx >= MGGS->AliveCharacters.Num())
			{
				idx = 0;
			}
		}
		SpectateOtherPlayer(idx);
	}
}