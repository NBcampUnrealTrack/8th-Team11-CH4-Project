// MGPlayerCharacter.cpp


#include "MGPlayerCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Component/MGFlagActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/GameStateBase.h"
#include "EngineUtils.h"
#include "Component/MGStatusComponent.h"
#include "Component/MGNameWidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Controller/MGPlayerController.h"
#include "GameState/MGGameStateBase.h"
#include "PlayerState/MGPlayerState.h"
#include "Type/MGPlayerColor.h"
#include "Type/MGTypes.h"
#include "NiagaraComponent.h"
#include "Animation/MGAnimInstanceBase.h"
#include "UI/MGFlagHUD.h"
#include "Component/Button/MGInteractionOverlapComponent.h"
#include "Net/UnrealNetwork.h"
#include "Minigames.h"

AMGPlayerCharacter::AMGPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	StatusComponent = CreateDefaultSubobject<UMGStatusComponent>(TEXT("StatusComponent"));

	NameWidgetComponent = CreateDefaultSubobject<UMGNameWidgetComponent>(TEXT("NameWidgetComponent"));
	NameWidgetComponent->SetupAttachment(GetRootComponent());
	NameWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	NameWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	NameWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InteractionComponent = CreateDefaultSubobject<UMGInteractionOverlapComponent>(TEXT("InteractionComponent"));

	//깃발뺏기 관련 컴포넌트 설정
	FlagActorComponent = CreateDefaultSubobject<UMGFlagActorComponent>(TEXT("FlagActorComponent"));

	FlagMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMeshComponent"));
	FlagMeshComponent->SetupAttachment(GetRootComponent());
	FlagMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlagMeshComponent->SetVisibility(false);

	FlagEffectMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagEffectMeshComponent"));
	FlagEffectMeshComponent->SetupAttachment(FlagMeshComponent);
	FlagEffectMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlagEffectMeshComponent->SetVisibility(false);

	FlagNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FlagNiagaraComponent"));
	FlagNiagaraComponent->SetupAttachment(FlagMeshComponent);
	FlagNiagaraComponent->SetAutoActivate(false);


	//버튼 게임 관련
	GetCharacterMovement()->bImpartBaseVelocityZ = false;

}

void AMGPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveInput);

	EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLookInput);

	EIC->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	if (IsValid(InteractionAction))
	{
		EIC->BindAction(InteractionAction, ETriggerEvent::Started, this, &ThisClass::HandleInteractionInput);
		EIC->BindAction(InteractionAction, ETriggerEvent::Completed, this, &ThisClass::HandleInteractionEndInput);
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (AMGFlagHUD* FlagHUD = Cast<AMGFlagHUD>(PC->GetHUD()))
		{
			FlagHUD->BindPlayerFlagComponent(FlagActorComponent);
		}
	}
}

void AMGPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerColorMat = GetMesh()->CreateDynamicMaterialInstance(0, GetMesh()->GetMaterial(0));

	if (IsLocallyControlled() == true)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		checkf(IsValid(PC) == true, TEXT("PlayerController is invalid."));

		UEnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		checkf(IsValid(EILPS) == true, TEXT("EnhancedInputLocalPlayerSubsystem is invalid."));

		EILPS->AddMappingContext(InputMappingContext, 0);
	}

	if (IsValid(FlagActorComponent))
	{
		FlagActorComponent->RegisterFlagMeshes(FlagMeshComponent, FlagEffectMeshComponent, FlagNiagaraComponent);
	}

}

void AMGPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentCamRot);
}

void AMGPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled() == true)
	{
		CurrentCamRot = Camera->GetComponentRotation();
		if ((CurrentCamRot-PreviousCamRot).IsNearlyZero() == false)
		{
			ServerRPCUpdateCamRot(CurrentCamRot);
			PreviousCamRot = CurrentCamRot;
		}
	}
	
	if (IsValid(NameWidgetComponent) == true && HasAuthority() == false)
	{
		FVector WidgetComponentLocation = NameWidgetComponent->GetComponentLocation();
		FVector LocalPlayerCameraLocation = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation();
		NameWidgetComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(WidgetComponentLocation, LocalPlayerCameraLocation));
	}
}

void AMGPlayerCharacter::FillPlayerColor()
{
	if (IsValid(PlayerColorMat))
	{
		AMGPlayerState* MGPS = GetPlayerState<AMGPlayerState>();
		if (IsValid(MGPS))
		{
			PlayerColorMat->SetVectorParameterValue("PlayerColor", MGPlayerColorToLinear(MGPS->PlayerColor));
		}
	}
}

void AMGPlayerCharacter::HandleMoveInput(const FInputActionValue& InValue)
{
	if (ensure(IsValid(Controller)) == false)
	{
		return;
	}

	const FVector2D InMovementVector = InValue.Get<FVector2D>();

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator ControlYawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, InMovementVector.X);
	AddMovementInput(RightDirection, InMovementVector.Y);
}

void AMGPlayerCharacter::HandleLookInput(const FInputActionValue& InValue)
{
	if (ensure(IsValid(Controller)) == false)
	{
		return;
	}

	const FVector2D InLookVector = InValue.Get<FVector2D>();

	AddControllerYawInput(InLookVector.X);
	AddControllerPitchInput(InLookVector.Y);
}

void AMGPlayerCharacter::HandleInteractionInput(const FInputActionValue& InValue)
{
	if (ensure(IsValid(Controller)) == false)
	{
		return;
	}

	AMGGameStateBase* MGGS = GetWorld()->GetGameState<AMGGameStateBase>();
	if (ensure(IsValid(MGGS)) == false)
	{
		return;
	}
	EMinigameType MGType = MGGS->GetCurrentMinigameType();
	
	switch (MGType)
	{
	case EMinigameType::PassBomb:
		if (IsCarrying)
		{
			ServerRPC_SendPlayPassBombRequest(true);

			if (HasAuthority() == false && IsLocallyControlled() == true)
			{
				PlayPassBombMontage(true);
			}
			
			//IsPlayingPassBomb = true;
		}
		break;
	case EMinigameType::FlagGame:
		if (IsValid(FlagActorComponent))
		{
			FlagActorComponent->ServerRPCTakeFlag();
		}
		break;
	}
}

void AMGPlayerCharacter::HandleInteractionEndInput(const FInputActionValue& InValue)
{
	if (ensure(IsValid(Controller)) == false)
	{
		return;
	}

	AMGGameStateBase* MGGS = GetWorld()->GetGameState<AMGGameStateBase>();
	if (ensure(IsValid(MGGS)) == false)
	{
		return;
	}
	EMinigameType MGType = MGGS->GetCurrentMinigameType();

	switch (MGType)
	{
	case EMinigameType::PassBomb:
		if (IsCarrying)
		{
			ServerRPC_SendPlayPassBombRequest(false);

			if (HasAuthority() == false && IsLocallyControlled() == true)
			{
				PlayPassBombMontage(false);
			}
			//IsPlayingPassBomb = false;
		}
		break;
	}
}

void AMGPlayerCharacter::PlayPassBombMontage(bool Value)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) == true)
	{
		FMontageBlendSettings BlendSetting;
		if (AnimInstance->Montage_IsActive(PassBombMontage))
		{
			BlendSetting.Blend.BlendTime = 0.f;
		}
		float InitPos = AnimInstance->Montage_IsActive(PassBombMontage) ? AnimInstance->Montage_GetPosition(PassBombMontage) : 0.f;
		AnimInstance->Montage_PlayWithBlendSettings(PassBombMontage, BlendSetting, Value ? 1.f : -1.f, EMontagePlayReturnType::MontageLength, InitPos);
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Magenta, FString::Printf(TEXT("InitPos: %f"), InitPos));

		if (HasAuthority() == true)
		{
			if (Value == true)
			{
				if (AnimInstance->Montage_GetPosition(PassBombMontage) >= 0.1f)
				{
					OnTryPassBombDelegate.Broadcast(true);
				}
			}
			else
			{
				OnTryPassBombDelegate.Broadcast(false);
			}
		}
	}
}

void AMGPlayerCharacter::ServerRPC_SendPlayPassBombRequest_Implementation(bool Value)
{
	PlayPassBombMontage(Value);

	for (APlayerController* PC : TActorRange<APlayerController>(GetWorld()))
	{
		if (IsValid(PC) == true && GetController() != PC)  // 이 캐릭터는 공격한 플레이어의 캐릭터임. 공격한 플레이어의 컨트롤러 외의 컨트롤러들을 찾기 위한 조건문.
		{
			AMGPlayerCharacter* OtherPlayerCharacter = Cast<AMGPlayerCharacter>(PC->GetPawn());
			if (OtherPlayerCharacter)
			{
				OtherPlayerCharacter->ClientRPC_PlayPassBombMontage(this, Value); // 다른 플레이어 컨트롤러의 캐릭터에 공격한 클라이언트의 캐릭터를 넘겨줘서, 애니메이션이 재생되게끔 함.
			}
		}
	}
}

void AMGPlayerCharacter::ClientRPC_PlayPassBombMontage_Implementation(AMGPlayerCharacter* TargetCharacter, bool Value)
{
	if (IsValid(TargetCharacter) == true)
	{
		TargetCharacter->PlayPassBombMontage(Value);
	}
}

void AMGPlayerCharacter::MulticastRPC_SetCarryState_Implementation(bool Value)
{
	IsCarrying = Value;
	if (IsValid(GetMesh()))
	{
		UMGAnimInstanceBase* AnimInstance = Cast<UMGAnimInstanceBase>(GetMesh()->GetAnimInstance());
		if (IsValid(AnimInstance) == true)
		{
			AnimInstance->bIsCarrying = Value;
		}

		if (IsCarrying == false && AnimInstance->GetCurrentActiveMontage() == PassBombMontage)
		{
			ServerRPC_SendPlayPassBombRequest(false);

			if (HasAuthority() == false && IsLocallyControlled() == true)
			{
				PlayPassBombMontage(false);
			}
		}
	}
}

void AMGPlayerCharacter::ServerRPCUpdateCamRot_Implementation(const FRotator& InCamRot)
{
	CurrentCamRot = InCamRot;
}