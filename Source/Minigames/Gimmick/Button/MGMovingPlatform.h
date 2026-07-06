#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGMovingPlatform.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

USTRUCT()
struct FStandCollisionInfo
{
    GENERATED_BODY()

    UPROPERTY()
    FVector RelativeLocation = FVector::ZeroVector;

    UPROPERTY()
    FVector Extent = FVector(10.f, 10.f, 5.f);
};

UCLASS()
class MINIGAMES_API AMGMovingPlatform : public AActor
{
    GENERATED_BODY()

public:
    AMGMovingPlatform();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category = "Platform")
    FVector GetSurfaceExtent() const;

    UFUNCTION(BlueprintCallable, Category = "Platform")
    FVector GetSurfaceTopLocation() const;

    void AddStandCollisionAt(const FVector& RelativeLocation, const FVector& Extent);
    void AddStandCollisionAtWorldTop(
        const FVector& WorldXY,
        float WorldTopZ,
        const FVector& Extent);

    static constexpr int32 MaxStandCollisions = 3;

protected:
    UPROPERTY(VisibleAnywhere, Category = "Platform")
    TObjectPtr<UStaticMeshComponent> PlatformMesh;

    UPROPERTY(VisibleAnywhere, Category = "Platform")
    TObjectPtr<UBoxComponent> SpawnArea;

    UPROPERTY(EditAnywhere, Category = "Movement")
    FVector MoveOffset = FVector(0.f, 0.f, 200.f);

    UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.1"))
    float Duration = 4.f;

private:
    void CreateStandCollisionComponents();

    float ComputePingPongAlpha(float ServerTime) const;

    UFUNCTION()
    void OnRep_StandCollisionInfos();
    void ApplyStandCollisionInfo(int32 Index);

private:
    FVector StartLocation = FVector::ZeroVector;

    UPROPERTY()
    TArray<TObjectPtr<UBoxComponent>> StandCollisions;

    UPROPERTY(ReplicatedUsing = OnRep_StandCollisionInfos)
    TArray<FStandCollisionInfo> StandCollisionInfos;

    /* //ÇÃ·§Æû À§Ä¡ È®ÀÎ¿ë ·Î±×
    public:
        UFUNCTION(NetMulticast, Unreliable)
        void MulticastDebugPlatform(const FVector& ServerLocation, float ServerTime);
    private:
        float DebugTimer = 0.f;
        */
};