// MGStatusComponent.h

#pragma once

#include "Components/ActorComponent.h"
#include "MGStatusComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentHPChangedDelegate, float /*InCurrentHP*/);
DECLARE_MULTICAST_DELEGATE(FOnOutOfCurrentHPDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxHPChangedDelegate, float /*InMaxHP*/);

class UMGEffectDataAsset;

// Speed 구조체
USTRUCT(BlueprintType)
struct FSpeedEffect
{
	GENERATED_BODY()

	// 고유 Key
	UPROPERTY()
	FGuid UniqueID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	const UMGEffectDataAsset* BuffData = nullptr;

	FTimerHandle TimerHandle;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MINIGAMES_API UMGStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMGStatusComponent();

	float ApplyDamage(float InDamage);

	float GetCurrentHP() const { return CurrentHP; }
	void SetCurrentHP(float InCurrentHP);

	float GetMaxHP() const { return MaxHP; }
	void SetMaxHP(float InMaxHP);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION()
	void OnRep_CurrentHP();

	UFUNCTION()
	void OnRep_MaxHP();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


public:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHP)
	float CurrentHP;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHP)
	float MaxHP;

	FOnCurrentHPChangedDelegate OnCurrentHPChanged;

	FOnOutOfCurrentHPDelegate OnOutOfCurrentHP;

	FOnMaxHPChangedDelegate OnMaxHPChanged;


#pragma region MovementSpeed

public:
	UFUNCTION(BlueprintCallable, Category = "Status|Speed")
	float GetOriginSpeed() const { return OriginSpeed; }

	UFUNCTION(BlueprintCallable, Category = "Status|Speed")
	float GetNormalSpeed() const { return NormalSpeed; }

	UFUNCTION(BlueprintCallable, Category = "Status|Speed")
	void SetNormalSpeed(float InSpeed);

	UFUNCTION(BlueprintCallable, Category = "Status|Speed")
	void AddEffectforDuration(const UMGEffectDataAsset* InEffectData);

protected:
	UFUNCTION()
	void OnRep_NormalSpeed();

	// 속도를 갱신하는 함수
	void UpdateSpeed();

	// 타이머가 끝났을 때 호출될 함수, 속도를 기존 속도로 갱신
	void OnSpeedEffectExpired(FGuid ExpiredEffectID);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status|Speed")
	float OriginSpeed;

	// NormalSpeed 값을 사용한 계산이 필요하다면 코드 변경 필요
	UPROPERTY(ReplicatedUsing = OnRep_NormalSpeed, VisibleAnywhere, BlueprintReadOnly, Category = "Status|Speed")
	float NormalSpeed;

private:
	UPROPERTY()
	TArray<FSpeedEffect> ActiveSpeedEffects; // 활성화된 속도 효과 리스트

#pragma endregion

};
