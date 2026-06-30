#include "Gimmick/Button/MGButtonActor.h"
#include "Character/MGPlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerState/MGPlayerState.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"

AMGButtonActor::AMGButtonActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    SetRootComponent(BaseMesh);
    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    ButtonMesh->SetupAttachment(BaseMesh);
}

void AMGButtonActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMGButtonActor, bPressed);
    DOREPLIFETIME(AMGButtonActor, CurrentColor);
}

void AMGButtonActor::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    ButtonOriginLocation = ButtonMesh->GetRelativeLocation();
}

void AMGButtonActor::BeginPlay()
{
    Super::BeginPlay();

    DynamicMaterial = ButtonMesh->CreateDynamicMaterialInstance(0);
    ApplyVisual();
}

void AMGButtonActor::BeginInteract_Implementation(AActor* Interactor)
{
    if (!HasAuthority())
    {
        return;
    }

    AMGPlayerCharacter* Player = Cast<AMGPlayerCharacter>(Interactor);
    if (!Player)
    {
        return;
    }

    AMGPlayerState* PlayerState = Player->GetPlayerState<AMGPlayerState>();
    if (!PlayerState)
    {
        return;
    }

    bPressed = true;
    CurrentColor = PlayerState->GetPlayerLinearColor();

    ApplyVisual();
}

void AMGButtonActor::EndInteract_Implementation(AActor* Interactor)
{
    if (!HasAuthority())
    {
        return;
    }

    bPressed = false;
    ApplyVisual();
}

void AMGButtonActor::OnRep_Pressed()
{
    ApplyVisual();
}

void AMGButtonActor::OnRep_Color()
{
    ApplyVisual();
}

void AMGButtonActor::ApplyVisual()
{
    const FVector TargetLocation = bPressed
        ? ButtonOriginLocation - FVector(0.f, 0.f, PressDepth)
        : ButtonOriginLocation;

    ButtonMesh->SetRelativeLocation(TargetLocation);

    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue(ColorParameterName, CurrentColor);
    }
}