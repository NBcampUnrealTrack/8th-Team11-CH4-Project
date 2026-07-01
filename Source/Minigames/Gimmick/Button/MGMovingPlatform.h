#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGMovingPlatform.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class UInterpToMovementComponent;

UCLASS()
class MINIGAMES_API AMGMovingPlatform : public AActor
{
    GENERATED_BODY()

public:
    AMGMovingPlatform();

    // 버튼 스포너가 플랫폼 표면 크기 참조용
    UFUNCTION(BlueprintCallable, Category = "Platform")
    FVector GetSurfaceExtent() const;

    // 버튼 스포너가 플랫폼 위 좌표 참조용
    UFUNCTION(BlueprintCallable, Category = "Platform")
    FVector GetSurfaceTopLocation() const;

protected:
    UPROPERTY(VisibleAnywhere, Category = "Platform")
    TObjectPtr<UStaticMeshComponent> PlatformMesh;

    // AMGButtonSpawner::GetSpawnArea 에서 "SpawnArea" 태그로 찾는 박스
    UPROPERTY(VisibleAnywhere, Category = "Platform")
    TObjectPtr<UBoxComponent> SpawnArea;

    // 이동 담당 컴포넌트 (C++에서 자동 부착, 값은 각 인스턴스 Details에서 조절)
    UPROPERTY(VisibleAnywhere, Category = "Movement")
    TObjectPtr<UInterpToMovementComponent> InterpMovement;

/* //플랫폼 위치 확인용 로그
Public: 
    UFUNCTION(NetMulticast, Unreliable)
    void MulticastDebugPlatform(const FVector& ServerLocation, float ServerTime);

    virtual void Tick(float DeltaSeconds) override;
private:
    float DebugTimer = 0.f;
    */

};
