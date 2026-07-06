#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGResetToPlayerStartTrigger.generated.h"

class UBoxComponent;
class APlayerStart;

UCLASS()
class AMGResetToPlayerStartTrigger : public AActor
{
    GENERATED_BODY()

public:
    AMGResetToPlayerStartTrigger();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
    TObjectPtr<UBoxComponent> TriggerVolume;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Trigger")
    TObjectPtr<APlayerStart> TargetPlayerStart;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
    FName PlayerStartTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
    float ResetCooldownSeconds = 1.0f;

    UFUNCTION()
    void OnTriggerOverlapBegin(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

private:

    TMap<TWeakObjectPtr<AActor>, float> LastResetTimeMap;

    APlayerStart* FindTargetPlayerStart() const;
};