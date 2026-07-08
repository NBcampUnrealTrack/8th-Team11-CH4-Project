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
    DOREPLIFETIME(AMGButtonActor, bColorOverridden);
    DOREPLIFETIME(AMGButtonActor, CurrentOwnerState);
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
    bColorOverridden = true;
    ApplyVisual();

    // 소유권 변경 + 점수 이전 (AMGButton에서 병합)
    SetButtonOwner(PlayerState);
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

bool AMGButtonActor::SetButtonOwner(APlayerState* NewOwnerState)
{
    // 유효성 검사
    if (!NewOwnerState)
    {
        UE_LOG(LogTemp, Error, TEXT("실패: NewOwnerState가 nullptr입니다!"));
        return false;
    }

    // 시스템 고유 ID 비교 - 이미 같은 주인이면 무시
    if (CurrentOwnerState != nullptr)
    {
        if (CurrentOwnerState->GetUniqueId() == NewOwnerState->GetUniqueId())
        {
            return false;
        }
    }

    // 이전 소유권자 개수 감소
    if (CurrentOwnerState != nullptr)
    {
        const float OldScore = CurrentOwnerState->GetScore();
        CurrentOwnerState->SetScore(FMath::Max(0.f, OldScore - 1.f));
    }

    // 새 소유권자 개수 증가
    NewOwnerState->SetScore(NewOwnerState->GetScore() + 1.f);

    // 소유권 이전
    CurrentOwnerState = NewOwnerState;

    UE_LOG(LogTemp, Log, TEXT("버튼 %s ➔ 새 주인: %s"),
        *GetName(), *NewOwnerState->GetPlayerName());

    return true;
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
    if (DynamicMaterial && bColorOverridden)
    {
        DynamicMaterial->SetVectorParameterValue(ColorParameterName, CurrentColor);
    }
}