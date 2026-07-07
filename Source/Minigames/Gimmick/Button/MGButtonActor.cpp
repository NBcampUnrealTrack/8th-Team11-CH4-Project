#include "Gimmick/Button/MGButtonActor.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

AMGButtonActor::AMGButtonActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMesh->SetupAttachment(BaseMesh);
}

void AMGButtonActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (ButtonMesh)
	{
		ButtonOriginLocation = ButtonMesh->GetRelativeLocation();
		DynamicMaterial = ButtonMesh->CreateAndSetMaterialInstanceDynamic(0);
	}
}

void AMGButtonActor::BeginPlay()
{
	Super::BeginPlay();
	ApplyVisual();
}

void AMGButtonActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMGButtonActor, bPressed);
	DOREPLIFETIME(AMGButtonActor, CurrentColor);
	DOREPLIFETIME(AMGButtonActor, bColorOverridden);
	DOREPLIFETIME(AMGButtonActor, CurrentOwnerState);
}

void AMGButtonActor::BeginInteract_Implementation(AActor* Interactor)
{
	if (!HasAuthority())
	{
		return;
	}

	bPressed = true;
	OnRep_Pressed();

	APlayerState* InteractorState = nullptr;
	if (const APawn* InteractorPawn = Cast<APawn>(Interactor))
	{
		InteractorState = InteractorPawn->GetPlayerState();
	}

	SetButtonOwner(InteractorState);
}

void AMGButtonActor::EndInteract_Implementation(AActor* Interactor)
{
	if (!HasAuthority())
	{
		return;
	}

	bPressed = false;
	OnRep_Pressed();
}

float AMGButtonActor::GetButtonTopWorldZ() const
{
	if (!ButtonMesh)
	{
		return GetActorLocation().Z;
	}
	return ButtonMesh->Bounds.Origin.Z + ButtonMesh->Bounds.BoxExtent.Z;
}

bool AMGButtonActor::SetButtonOwner(APlayerState* NewOwnerState)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("SetButtonOwner는 서버에서만 호출해야 합니다. (%s)"), *GetName());
		return false;
	}

	if (!NewOwnerState)
	{
		UE_LOG(LogTemp, Error, TEXT("실패: NewOwnerState가 nullptr입니다! (%s)"), *GetName());
		return false;
	}

	if (CurrentOwnerState && CurrentOwnerState->GetUniqueId() == NewOwnerState->GetUniqueId())
	{
		UE_LOG(LogTemp, Log, TEXT("무시: 이미 이 버튼의 주인입니다. (%s)"), *NewOwnerState->GetPlayerName());
		return false;
	}

	if (CurrentOwnerState)
	{
		const float OldScore = CurrentOwnerState->GetScore();
		CurrentOwnerState->SetScore(FMath::Max(0.f, OldScore - 1.f));
	}

	NewOwnerState->SetScore(NewOwnerState->GetScore() + 1.f);

	CurrentOwnerState = NewOwnerState;

	UE_LOG(LogTemp, Log, TEXT("버튼 %s ➔ 새 주인: %s"), *GetName(), *NewOwnerState->GetPlayerName());

	if (!bColorOverridden)
	{
		CurrentColor = GetColorForOwner(NewOwnerState);
		OnRep_Color();
	}

	OnButtonColorChanged(CurrentOwnerState);

	return true;
}

void AMGButtonActor::SetButtonColor(FLinearColor NewColor)
{
	if (!HasAuthority())
	{
		return;
	}

	bColorOverridden = true;
	CurrentColor = NewColor;
	OnRep_Color();
}

FLinearColor AMGButtonActor::GetColorForOwner_Implementation(APlayerState* OwnerState) const
{
	if (!OwnerState)
	{
		return FLinearColor::Black;
	}

	const uint32 Hash = GetTypeHash(OwnerState->GetPlayerId());
	const float Hue = static_cast<float>(Hash % 360);
	return FLinearColor::MakeFromHSV8(static_cast<uint8>(Hue / 360.f * 255.f), 255, 255);
}

void AMGButtonActor::ApplyVisual()
{
	if (ButtonMesh)
	{
		FVector NewLocation = ButtonOriginLocation;
		if (bPressed)
		{
			NewLocation.Z -= PressDepth;
		}
		ButtonMesh->SetRelativeLocation(NewLocation);
	}

	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(ColorParameterName, CurrentColor);
	}
}

void AMGButtonActor::OnRep_Pressed()
{
	ApplyVisual();
}

void AMGButtonActor::OnRep_Color()
{
	ApplyVisual();
}

void AMGButtonActor::OnRep_CurrentOwnerState()
{
	OnButtonColorChanged(CurrentOwnerState);
}