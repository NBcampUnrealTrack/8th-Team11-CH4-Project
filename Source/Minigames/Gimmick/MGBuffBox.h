// MGBuffBox.h

#pragma once

#include "GameFramework/Actor.h"
#include "MGBuffBox.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UParticleSystemComponent;
class UMGEffectDataAsset;

UCLASS()
class MINIGAMES_API AMGBuffBox : public AActor
{
	GENERATED_BODY()

public:
	AMGBuffBox();

	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	UFUNCTION()
	void OnEffectFinished(UParticleSystemComponent* ParticleSystem);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnBoxConsumed();

protected:
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	TObjectPtr<UParticleSystemComponent> ParticleEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effect")
	TArray<TObjectPtr<UMGEffectDataAsset>> ItemEffectDataArray;

};
