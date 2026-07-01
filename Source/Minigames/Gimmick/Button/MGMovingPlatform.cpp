#include "Gimmick/Button/MGMovingPlatform.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InterpToMovementComponent.h"

#include "GameFramework/GameStateBase.h"

AMGMovingPlatform::AMGMovingPlatform()
{
    PrimaryActorTick.bCanEverTick = false;
    //PrimaryActorTick.bCanEverTick = true; //플랫폼 위치 동기화 확인용

    bReplicates = true;

    // 스포너가 GetAllActorsWithTag(TEXT("MovingPlatform"))로 찾을 수 있게 태그 부여
    Tags.Add(TEXT("MovingPlatform"));

    PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
    SetRootComponent(PlatformMesh);
    PlatformMesh->SetMobility(EComponentMobility::Movable);
    PlatformMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    PlatformMesh->SetCollisionResponseToAllChannels(ECR_Block);

    SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
    SpawnArea->SetupAttachment(PlatformMesh);
    SpawnArea->SetBoxExtent(FVector(150.f, 150.f, 10.f));
    SpawnArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SpawnArea->ComponentTags.Add(TEXT("SpawnArea"));

    InterpMovement = CreateDefaultSubobject<UInterpToMovementComponent>(TEXT("InterpMovement"));
    // UpdatedComponent는 지정 안 해도 RootComponent(PlatformMesh)로 자동 연결됨
    InterpMovement->SetIsReplicated(true);   // 명시적으로 한 번 더 보장
    InterpMovement->Duration = 4.f;
    InterpMovement->BehaviourType = EInterpToBehaviourType::PingPong;

    // 기본 이동 경로 (시작 위치 기준 상대좌표, Z로 200 위까지 왕복)
    // -> 실제 값은 각 Blueprint/인스턴스의 Details 패널에서 자유롭게 덮어써도 됨
    InterpMovement->AddControlPointPosition(FVector::ZeroVector, true);
    InterpMovement->AddControlPointPosition(FVector(0.f, 0.f, 200.f), true);
}

FVector AMGMovingPlatform::GetSurfaceExtent() const
{
    return PlatformMesh ? PlatformMesh->Bounds.BoxExtent : FVector::ZeroVector;
}

FVector AMGMovingPlatform::GetSurfaceTopLocation() const
{
    if (PlatformMesh)
    {
        const FVector Origin = PlatformMesh->Bounds.Origin;
        const float HalfHeight = PlatformMesh->Bounds.BoxExtent.Z;
        return FVector(Origin.X, Origin.Y, Origin.Z + HalfHeight);
    }
    return GetActorLocation();
}

/* //플랫폼 위치 확인 로그
void AMGMovingPlatform::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!HasAuthority())
        return;

    DebugTimer += DeltaSeconds;

    if (DebugTimer >= 0.5f)
    {
        DebugTimer = 0.f;

        float ServerTime =
            GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

        MulticastDebugPlatform(GetActorLocation(), ServerTime);
    }
}

void AMGMovingPlatform::MulticastDebugPlatform_Implementation(
    const FVector& ServerLocation,
    float ServerTime)
{
    FVector MyLocation = GetActorLocation();

    const float Error =
        FVector::Dist(ServerLocation, MyLocation);

    UE_LOG(LogTemp, Warning,
        TEXT("[%s] %s | ServerTime=%.3f | Server=%s | Mine=%s | Error=%.3f"),
        HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"),
        *GetName(),
        ServerTime,
        *ServerLocation.ToCompactString(),
        *MyLocation.ToCompactString(),
        Error);
}
*/