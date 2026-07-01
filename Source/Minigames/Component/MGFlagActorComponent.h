#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MGFlagActorComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MINIGAMES_API UMGFlagActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMGFlagActorComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 깃발 메시와 이펙트 메시를 등록
	void RegisterFlagMeshes(class UStaticMeshComponent* InFlagMesh, class UStaticMeshComponent* InFlagEffectMesh);

	bool SetHasFlag(bool bHasFlag);

	FORCEINLINE bool GetHasFlag() const { return bFlagState; }
	FORCEINLINE bool GetIsFlagProtected() const { return bIsFlagProtected; }

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCTakeFlag();

private:
	UFUNCTION()
	void OnRep_FlagState();

	UFUNCTION()
	void OnRep_IsFlagProtected();

	void ClearFlagProtection();

private:
	UPROPERTY(ReplicatedUsing = OnRep_FlagState)
	bool bFlagState = false;

	UPROPERTY(ReplicatedUsing = OnRep_IsFlagProtected)
	bool bIsFlagProtected = false;

	FTimerHandle FlagProtectTimerHandle;

	// 캐릭터에게 전달받을 메시 컴포넌트 포인터
	UPROPERTY()
	TObjectPtr<class UStaticMeshComponent> FlagMeshComp;

	UPROPERTY()
	TObjectPtr<class UStaticMeshComponent> FlagEffectMeshComp;
};