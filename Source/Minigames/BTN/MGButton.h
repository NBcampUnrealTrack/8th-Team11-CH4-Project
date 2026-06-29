#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGButton.generated.h"

UCLASS()
class MINIGAMES_API AMGButton : public AActor
{
    GENERATED_BODY()
    
public:	
    AMGButton();

    // 플레이어가 근처에서 키를 누르면 호출될 함수
    UFUNCTION(BlueprintCallable, Category = "Gameplay|Button")
    bool SetButtonOwner(class APlayerState* NewOwnerState);

    // 버튼을 소유하고 있는 플레이어 상태를 반환 
    UFUNCTION(BlueprintPure, Category = "Gameplay|Button")
    class APlayerState* GetCurrentOwner() const { return CurrentOwnerState; }

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;

    // 버튼 메쉬와 충돌 영역
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* ButtonMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UBoxComponent* InteractionCollision;

    // 플레이어가 버튼 근처에 진입/퇴장 시 호출할 함수 
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    // 버튼 색상 변경 이벤트 호출
    UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay|Button")
    void OnButtonColorChanged(class APlayerState* NewOwnerState);

private:
    // 서버에서 소유권 변경되면 클라이언트에도 변경 
    UPROPERTY(ReplicatedUsing = OnRep_CurrentOwnerState, VisibleAnywhere, Category = "Gameplay|Button")
    class APlayerState* CurrentOwnerState = nullptr;

    // 값이 복제될 때 클라이언트에서 자동으로 실행
    UFUNCTION()
    void OnRep_CurrentOwnerState();
};