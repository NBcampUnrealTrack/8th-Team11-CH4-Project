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
	// TargetHolder에 MGBombActor를 부착하는 함수
	void AttachToHolder(ACharacter* TargetHolder);

	// Timer에 의해 호출될 bCanPass를 true로 만들어주는 함수
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
	// 폭탄을 소유한 Character, 서버에서 BombHolder가 바뀌면 Replication되고
	// Replication된 값을 받은 각 Client에서 OnRep_BombHolder()가 호출됨
	UPROPERTY(ReplicatedUsing = OnRep_BombHolder)
	ACharacter* BombHolder;

	// 폭탄을 붙일 SocketName
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	FName AttachSocketName;

	// 다시 Pass하기 위한 쿨타임 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	float PassCooldownTime;

	// DebugSphere를 Draw 할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	bool bShowDebugSphere = true;

private:
	// 폭탄을 Pass할 수 있는지 여부
	bool bCanPass;

	// 다시 Pass에 필요한 쿨타임 관리 Timer
	FTimerHandle PassCooldownTimer;
};
