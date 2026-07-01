#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGFlagActor.generated.h"

UCLASS()
class MINIGAMES_API AMGFlagActor : public AActor
{
	GENERATED_BODY()

public:
	AMGFlagActor();

protected:
	virtual void BeginPlay() override;

	// 오버랩 이벤트
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult);

private:
	// 콜리전 스피어 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereComp;

	// 깃발 스태틱 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* FlagMesh;
};