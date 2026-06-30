#include "Gimmick/Button/MGButtonActor.h"
#include "Character/MGPlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerState/MGPlayerState.h"
#include "Materials/MaterialInstanceDynamic.h"

AMGButtonActor::AMGButtonActor()
{
    PrimaryActorTick.bCanEverTick = false;

    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    SetRootComponent(BaseMesh);

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    ButtonMesh->SetupAttachment(BaseMesh);
}

void AMGButtonActor::BeginPlay()
{
    Super::BeginPlay();

    ButtonOriginLocation = ButtonMesh->GetRelativeLocation();
    DynamicMaterial = ButtonMesh->CreateDynamicMaterialInstance(0);

    ApplyVisual();
}

void AMGButtonActor::BeginInteract_Implementation(AActor* Interactor)
{
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
    bPressed = false;
    ApplyVisual();
}

void AMGButtonActor::ApplyVisual()
{
    if (bPressed)
    {
        ButtonMesh->SetRelativeLocation(ButtonOriginLocation - FVector(0.f, 0.f, PressDepth));
    }
    else
    {
        ButtonMesh->SetRelativeLocation(ButtonOriginLocation);
    }

    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue(ColorParameterName,CurrentColor);
    }
}