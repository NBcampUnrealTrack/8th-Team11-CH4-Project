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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnBoxConsumed();

	// 서버 + 모든 클라이언트에게 상자가 다시 나타났음을 알리는 함수
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RespawnBox();

	// 실제로 리스폰 하는 함수
	void RespawnBox();

protected:
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	TObjectPtr<UParticleSystemComponent> ParticleEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effect")
	TArray<TObjectPtr<UMGEffectDataAsset>> ItemEffectDataArray;

	UPROPERTY(EditAnywhere, Category = "Item|Effect")
	float RespawnTime;

	FTimerHandle RespawnTimerHandler;
};
