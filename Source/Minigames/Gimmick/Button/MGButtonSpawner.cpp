#include "Gimmick/Button/MGButtonSpawner.h"

#include "Gimmick/Button/MGButtonActor.h"
#include "Gimmick/Button/MGMovingPlatform.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AMGButtonSpawner::AMGButtonSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
}

void AMGButtonSpawner::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        SpawnButtons();
    }
}

void AMGButtonSpawner::SpawnButtons()
{
    TArray<AActor*> Platforms;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("SpawnPlatform"), Platforms);

    if (Platforms.IsEmpty())
    {
        return;
    }

    for (const auto& Pair : BuildSpawnCounts(Platforms))
    {
        SpawnButtonsOnPlatform(Pair.Key, Pair.Value);
    }
}

TMap<AActor*, int32> AMGButtonSpawner::BuildSpawnCounts(const TArray<AActor*>& Platforms) const
{
    TMap<AActor*, int32> SpawnCounts;

    int32 Remaining = FMath::Min(TotalButtonCount, Platforms.Num() * MaxButtonsPerPlatform);

    for (AActor* Platform : Platforms)
    {
        if (Remaining-- <= 0)
        {
            break;
        }
        SpawnCounts.Add(Platform, 1);
    }

    while (Remaining > 0)
    {
        int32& Count = SpawnCounts.FindOrAdd(Platforms[FMath::RandRange(0, Platforms.Num() - 1)]);
        if (Count < MaxButtonsPerPlatform)
        {
            ++Count;
            --Remaining;
        }
    }

    return SpawnCounts;
}

void AMGButtonSpawner::SpawnButtonsOnPlatform(AActor* Platform, int32 ButtonCount)
{
    UBoxComponent* SpawnArea = GetSpawnArea(Platform);
    if (!SpawnArea)
    {
        return;
    }

    AMGMovingPlatform* MovingPlatform = Cast<AMGMovingPlatform>(Platform);
    TArray<FVector> SpawnedLocations;

    for (int32 i = 0; i < ButtonCount; ++i)
    {
        FVector SpawnLocation;
        if (!FindSpawnLocation(SpawnArea, SpawnedLocations, SpawnLocation))
        {
            continue;
        }

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        AMGButtonActor* Button = GetWorld()->SpawnActor<AMGButtonActor>(
            ButtonClass, SpawnLocation, FRotator::ZeroRotator, Params);

        if (!Button)
        {
            continue;
        }

        Button->AttachToComponent(Platform->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

        if (MovingPlatform)
        {
            MovingPlatform->AddStandCollisionAtWorldTop(
                FVector(SpawnLocation.X, SpawnLocation.Y, 0.f),
                Button->GetButtonTopWorldZ(),
                ButtonStandCollisionExtent);
        }

        SpawnedLocations.Add(SpawnLocation);
    }
}

UBoxComponent* AMGButtonSpawner::GetSpawnArea(AActor* Platform) const
{
    TArray<UBoxComponent*> Boxes;
    Platform->GetComponents<UBoxComponent>(Boxes);

    for (UBoxComponent* Box : Boxes)
    {
        if (Box->ComponentHasTag(TEXT("SpawnArea")))
        {
            return Box;
        }
    }
    return nullptr;
}

bool AMGButtonSpawner::FindSpawnLocation(
    UBoxComponent* SpawnArea,
    const TArray<FVector>& ExistingLocations,
    FVector& OutLocation) const
{
    const FVector Origin = SpawnArea->GetComponentLocation();
    const FVector Extent = SpawnArea->GetScaledBoxExtent();

    constexpr int32 MaxTry = 100;

    for (int32 Try = 0; Try < MaxTry; ++Try)
    {
        const FVector Candidate(
            Origin.X + FMath::FRandRange(-Extent.X, Extent.X),
            Origin.Y + FMath::FRandRange(-Extent.Y, Extent.Y),
            Origin.Z);

        bool bTooClose = false;

        for (const FVector& Pos : ExistingLocations)
        {
            if (FVector::Dist2D(Pos, Candidate) < MinDistance)
            {
                bTooClose = true;
                break;
            }
        }

        if (!bTooClose)
        {
            OutLocation = Candidate;
            return true;
        }
    }
    return false;
}