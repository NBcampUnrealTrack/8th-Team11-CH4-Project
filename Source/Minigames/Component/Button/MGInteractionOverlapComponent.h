#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MGInteractionOverlapComponent.generated.h"

class UPrimitiveComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MINIGAMES_API UMGInteractionOverlapComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    virtual void BeginPlay() override;

private:

    UFUNCTION()
    void OnBeginOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnEndOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex);
};