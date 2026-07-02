#include "Gimmick/Button/MGButtonActor.h"
#include "Character/MGPlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerState/MGPlayerState.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

AMGButtonActor::AMGButtonActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    SetRootComponent(BaseMesh);
    BaseMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BaseMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    ButtonMesh->SetupAttachment(BaseMesh);
    ButtonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

    GetWorldTimerManager().SetTimerForNextTick(this, &AMGButtonActor::ApplyVisual);

    FTimerHandle ReapplyHandle;
    GetWorldTimerManager().SetTimer(
        ReapplyHandle, this, &AMGButtonActor::ApplyVisual, 0.5f, false);
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

float AMGButtonActor::GetButtonTopWorldZ() const
{
    if (!ButtonMesh)
    {
        return GetActorLocation().Z;
    }

    const FVector WorldOrigin =
        BaseMesh->GetComponentTransform().TransformPosition(ButtonOriginLocation);

    const float MeshHalfHeight = ButtonMesh->GetStaticMesh()
        ? ButtonMesh->GetStaticMesh()->GetBounds().BoxExtent.Z * ButtonMesh->GetComponentScale().Z
        : 0.f;

    return WorldOrigin.Z + MeshHalfHeight;
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