#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGButtonSpawner.generated.h"

class AMGButtonActor;
class AMGMovingPlatform;
class UBoxComponent;

UCLASS()
class MINIGAMES_API AMGButtonSpawner : public AActor
{
    GENERATED_BODY()

public:
    AMGButtonSpawner();

protected:
    virtual void BeginPlay() override;

private:
    void SpawnButtons();

    TMap<AActor*, int32> BuildSpawnCounts(const TArray<AActor*>& Platforms) const;
    void SpawnButtonsOnPlatform(AActor* Platform, int32 ButtonCount);

    UBoxComponent* GetSpawnArea(AActor* Platform) const;
    bool FindSpawnLocation(
        UBoxComponent* SpawnArea,
        const TArray<FVector>& ExistingLocations,
        FVector& OutLocation) const;

private:
    UPROPERTY(EditAnywhere, Category = "Spawn")
    TSubclassOf<AMGButtonActor> ButtonClass;

    UPROPERTY(EditAnywhere, Category = "Spawn")
    int32 TotalButtonCount = 15;

    UPROPERTY(EditAnywhere, Category = "Spawn")
    float MinDistance = 120.f;

    UPROPERTY(EditAnywhere, Category = "Spawn")
    int32 MaxButtonsPerPlatform = 3;

    UPROPERTY(EditAnywhere, Category = "Spawn")
    FVector ButtonStandCollisionExtent = FVector(10.f, 10.f, 5.f);
};