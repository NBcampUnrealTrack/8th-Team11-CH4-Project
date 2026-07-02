#include "Gimmick/Button/MGMovingPlatform.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

AMGMovingPlatform::AMGMovingPlatform()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

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

    StandCollisions.Reserve(MaxStandCollisions);
    for (int32 i = 0; i < MaxStandCollisions; ++i)
    {
        const FName BoxName = *FString::Printf(TEXT("StandCollision%d"), i);

        UBoxComponent* Box = CreateDefaultSubobject<UBoxComponent>(BoxName);
        Box->SetupAttachment(PlatformMesh);
        Box->SetMobility(EComponentMobility::Movable);
        Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Box->SetCollisionResponseToAllChannels(ECR_Ignore);
        Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
        Box->CanCharacterStepUpOn = ECB_Yes;

        StandCollisions.Add(Box);
    }

}

void AMGMovingPlatform::BeginPlay()
{
    Super::BeginPlay();

    StartLocation = GetActorLocation();
}

void AMGMovingPlatform::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    const AGameStateBase* GameState = GetWorld()->GetGameState();
    if (!GameState)
    {
        return;
    }

    const float ServerTime = GameState->GetServerWorldTimeSeconds();
    const float CycleTime = FMath::Fmod(ServerTime, Duration * 2.f);

    const float Alpha =
        (CycleTime <= Duration)
        ? (CycleTime / Duration)
        : (2.f - CycleTime / Duration);

    const FVector NewLocation = StartLocation + MoveOffset * Alpha;

    SetActorLocation(NewLocation);
}

void AMGMovingPlatform::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMGMovingPlatform, StandCollisionInfos);
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

void AMGMovingPlatform::AddStandCollisionAt(const FVector& RelativeLocation, const FVector& Extent)
{

    if (!HasAuthority())
    {
        return;
    }

    if (StandCollisionInfos.Num() >= MaxStandCollisions)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[%s] StandCollision limit (%d) exceeded, ignoring"),
            *GetName(), MaxStandCollisions);
        return;
    }

    FStandCollisionInfo Info;
    Info.RelativeLocation = RelativeLocation;
    Info.Extent = Extent;

    StandCollisionInfos.Add(Info);  

    ApplyStandCollisionInfo(StandCollisionInfos.Num() - 1);
}

void AMGMovingPlatform::AddStandCollisionAtWorldTop(
    const FVector& WorldXY,
    float WorldTopZ,
    const FVector& Extent)
{
    const FVector WorldCenter(WorldXY.X, WorldXY.Y, WorldTopZ - Extent.Z);
    const FVector RelativeLocation =
        PlatformMesh->GetComponentTransform().InverseTransformPosition(WorldCenter);

    AddStandCollisionAt(RelativeLocation, Extent);
}

void AMGMovingPlatform::OnRep_StandCollisionInfos()
{
    for (int32 i = 0; i < StandCollisionInfos.Num(); ++i)
    {
        ApplyStandCollisionInfo(i);
    }
}

void AMGMovingPlatform::ApplyStandCollisionInfo(int32 Index)
{
    if (!StandCollisions.IsValidIndex(Index) ||
        !StandCollisionInfos.IsValidIndex(Index))
    {
        return;
    }

    UBoxComponent* Box = StandCollisions[Index];
    if (!Box)
    {
        return;
    }

    const FStandCollisionInfo& Info = StandCollisionInfos[Index];

    Box->SetRelativeLocation(Info.RelativeLocation);
    Box->SetBoxExtent(Info.Extent);
    Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

/* //플랫폼 위치 확인 로그
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