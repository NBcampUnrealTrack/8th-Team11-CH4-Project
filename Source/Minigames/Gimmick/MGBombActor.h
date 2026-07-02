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

	// BombHolder 값이 바뀌었을 때 호출될 OnRep 함수
	UFUNCTION()
	void OnRep_BombHolder();

	// 폭탄이 터질 때 실행될 함수, [폭발 나이아가라 이펙트, 사운드] 등
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_OnExplode();

	// 폭탄의 ExplodeTimer를 관리, GameMode에서 최초로 폭탄을 넘기는 함수
	void ActivateBomb(ACharacter* InitialHolder, float ExplodeTime);

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	// NewHolder를 BombHolder로 변경하는 함수
	void SetBombHolder(ACharacter* NewHolder);

	// TargetHolder에 MGBombActor를 부착하는 함수
	void AttachToHolder(ACharacter* TargetHolder);

	// Timer에 의해 호출될 bCanPass를 true로 만들어주는 함수
	void ResetPassCooldown();

	// ExplodeTimer 경과 후 폭탄이 터질 때 실행될 함수
	void ExplodeBomb();

public:
	//------------------------------------------------ Static
	// BombMesh를 부착할 위치
	UPROPERTY() 
	USceneComponent* Root;

	// BombActor의 Static Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	UStaticMeshComponent* BombMesh;

	// BombActor를 다른 사람에게 부착하기 위해 Collision 감지용 Sphere Component
	UPROPERTY() 
	class USphereComponent* PassTrigger;

	// PassTrigger의 반지름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	float PassTriggerRadius;

	// 폭탄을 붙일 SocketName
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	FName AttachSocketName;

	// Debug 할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	bool bShowDebug = true;

	//------------------------------------------------ Dynamic
	// 폭탄을 소유한 Character, 서버에서 BombHolder가 바뀌면 Replication되고
	// Replication된 값을 받은 각 Client에서 OnRep_BombHolder()가 호출됨
	// OnRep를 이용하면 Culling 등으로 무시될 일 없이 반드시 실행됨
	UPROPERTY(ReplicatedUsing = OnRep_BombHolder)
	ACharacter* BombHolder;

	// 다시 Pass하기 위한 쿨타임 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	float PassCooldownTime;

private:
	// 폭탄을 Pass할 수 있는지 여부
	bool bCanPass;

	// 다시 Pass에 필요한 쿨타임 관리 Timer
	FTimerHandle PassCooldownTimer;

	// 폭탄 활성화 시간 관리 Timer
	FTimerHandle ExplodeTimer;
};
