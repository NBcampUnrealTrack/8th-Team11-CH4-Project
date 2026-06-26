// MGBombActor.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGBombActor.generated.h"

UCLASS()
class MINIGAMES_API AMGBombActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMGBombActor();

	// virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnRep_BombHolder();


	void SetBombHolder(ACharacter* NewHolder);

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	void AttachToCurrentHolder();

	// Pass
	void ResetPassCooldown();

public:
	// BombMesh를 부착할 위치
	UPROPERTY() 
	USceneComponent* Root;

	// BombActor의 Static Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	UStaticMeshComponent* BombMesh;

	// BombActor를 다른 사람에게 부착하기 위해 Collision 감지용 Sphere Component
	UPROPERTY() 
	class USphereComponent* PassTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	float PassTriggerRadius;

	//----------------------------------------------------------
	// 폭탄을 소유한 Character
	UPROPERTY(ReplicatedUsing = OnRep_BombHolder)
	ACharacter* BombHolder;

private:
	bool bCanPass;

	// 다시 Pass에 필요한 쿨타임 관리 Timer
	FTimerHandle PassCooldownTimer;

	// 다시 Pass하기 위한 시간 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	float PassCooldownTime;
};
