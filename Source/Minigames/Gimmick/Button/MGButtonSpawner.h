#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGButtonSpawner.generated.h"

class AMGButtonActor;
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
};