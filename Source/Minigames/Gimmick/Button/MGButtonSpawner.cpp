#include "Gimmick/Button/MGButtonSpawner.h"

#include "Gimmick/Button/MGButtonActor.h"

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

    if (!HasAuthority())
    {
        return;
    }

    SpawnButtons();
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
        FVector Candidate;

        Candidate.X = Origin.X + FMath::FRandRange(-Extent.X, Extent.X);
        Candidate.Y = Origin.Y + FMath::FRandRange(-Extent.Y, Extent.Y);
        Candidate.Z = Origin.Z;

        bool bValid = true;

        for (const FVector& Pos : ExistingLocations)
        {
            if (FVector::Dist2D(Pos, Candidate) < MinDistance)
            {
                bValid = false;
                break;
            }
        }

        if (bValid)
        {
            OutLocation = Candidate;
            return true;
        }
    }

    return false;
}

void AMGButtonSpawner::SpawnButtons()
{
    TArray<AActor*> Platforms;

    UGameplayStatics::GetAllActorsWithTag(
        GetWorld(),
        TEXT("MovingPlatform"),
        Platforms);

    if (Platforms.IsEmpty())
    {
        return;
    }

    // 플랫폼 수 체크

    int32 MaxButtonCount = Platforms.Num() * 3;

    TotalButtonCount = FMath::Min(TotalButtonCount, MaxButtonCount);

    // 최소 1개씩 배정

    TMap<AActor*, int32> SpawnCounts;

    int32 Remaining = TotalButtonCount;

    for (AActor* Platform : Platforms)
    {
        if (Remaining <= 0)
            break;

        SpawnCounts.Add(Platform, 1);
        Remaining--;
    }

    // 남은 버튼 랜덤 추가

    while (Remaining > 0)
    {
        AActor* Platform =
            Platforms[FMath::RandRange(0, Platforms.Num() - 1)];

        int32& Count = SpawnCounts.FindOrAdd(Platform);

        if (Count < 3)
        {
            Count++;
            Remaining--;
        }
    }

    // Spawn

    for (const auto& Pair : SpawnCounts)
    {
        AActor* Platform = Pair.Key;

        UBoxComponent* SpawnArea = GetSpawnArea(Platform);

        if (!SpawnArea)
        {
            continue;
        }

        TArray<FVector> SpawnedLocations;

        for (int32 i = 0; i < Pair.Value; ++i)
        {
            FVector SpawnLocation;

            if (!FindSpawnLocation(
                SpawnArea,
                SpawnedLocations,
                SpawnLocation))
            {
                continue;
            }

            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride =
                ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            AMGButtonActor* Button =
                GetWorld()->SpawnActor<AMGButtonActor>(
                    ButtonClass,
                    SpawnLocation,
                    FRotator::ZeroRotator,
                    Params);

            if (!Button)
            {
                continue;
            }

            Button->AttachToComponent(
                Platform->GetRootComponent(),
                FAttachmentTransformRules::KeepWorldTransform);

            SpawnedLocations.Add(SpawnLocation);
        }
    }
}