#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MGFlagActorComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFlagStateChanged, bool bFlagState);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnFlagProtectionChanged, bool bIsFlagProtected);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MINIGAMES_API UMGFlagActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMGFlagActorComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 깃발 메시 등록
	void RegisterFlagMeshes(class UStaticMeshComponent* InFlagMesh, class UStaticMeshComponent* InFlagEffectMesh, class UNiagaraComponent* InFlagNiagara);

	// [기능 변경] 깃발 세팅 시 액터 포인터도 함께 받도록 변경
	bool SetHasFlag(bool bHasFlag, class AMGFlagActor* InFlagActor = nullptr);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetHasFlag() const { return bFlagState; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsFlagProtected() const { return bIsFlagProtected; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetStealRange() const { return StealRange; }

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCTakeFlag();

	FOnFlagStateChanged OnFlagStateChanged;
	FOnFlagProtectionChanged OnFlagProtectionChanged;

private:

	// 색 정보를 같이 넘겨줄 수 있도록 기존 OnRep_FlagState() 함수를 대체
	UFUNCTION()
	void OnRep_FlagVisuals();

	UFUNCTION()
	void OnRep_IsFlagProtected();

	void ClearFlagProtection();

private:
	UPROPERTY(ReplicatedUsing = OnRep_FlagVisuals)
	bool bFlagState = false;

	UPROPERTY(ReplicatedUsing = OnRep_IsFlagProtected)
	bool bIsFlagProtected = false;

	const float StealRange = 100.f;

	FTimerHandle FlagProtectTimerHandle;

	// 캐릭터에게 전달받을 메시 컴포넌트 포인터
	UPROPERTY()
	TObjectPtr<class UStaticMeshComponent> FlagMeshComp;

	UPROPERTY()
	TObjectPtr<class UStaticMeshComponent> FlagEffectMeshComp;

	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> FlagNiagaraComp;

	// 획득한 깃발에 캐릭터 색상을 반영하기 위한 액터 포인터 레플리케이션
	UPROPERTY(ReplicatedUsing = OnRep_FlagVisuals)
	FLinearColor ReplicatedFlagColor;

};