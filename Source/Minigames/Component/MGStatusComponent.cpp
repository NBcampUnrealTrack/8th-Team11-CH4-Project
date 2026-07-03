// MGStatusComponent.cpp


#include "Component/MGStatusComponent.h"

#include "Net/UnrealNetwork.h"
#include "Minigames.h"				// 매크로 사용

#include "Character/MGPlayerCharacter.h"				// 이동속도
#include "GameFramework/CharacterMovementComponent.h"	// 이동속도

UMGStatusComponent::UMGStatusComponent()
	: CurrentHP(100.f)
	, MaxHP(100.f)
	, OriginSpeed(600.0f)
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

float UMGStatusComponent::ApplyDamage(float InDamage)
{
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		return 0.f;
	}

	const float PreviousHP = CurrentHP;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, PreviousHP);

	SetCurrentHP(PreviousHP - ActualDamage);

	return ActualDamage;
}

void UMGStatusComponent::SetCurrentHP(float InCurrentHP)
{
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		return;
	}

	CurrentHP = InCurrentHP;
	if (CurrentHP <= KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.f;
		OnOutOfCurrentHP.Broadcast();
	}
	OnCurrentHPChanged.Broadcast(CurrentHP);
}

void UMGStatusComponent::SetMaxHP(float InMaxHP)
{
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		return;
	}

	MaxHP = InMaxHP;

	if (MaxHP < KINDA_SMALL_NUMBER)
	{
		MaxHP = 0.1f;
	}

	OnMaxHPChanged.Broadcast(MaxHP);
}

void UMGStatusComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentHP);
	DOREPLIFETIME_CONDITION(ThisClass, MaxHP, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, NormalSpeed, COND_OwnerOnly);
	// NormalSpeed 값을 사용한 계산이 필요하다면 코드 변경 필요
}

void UMGStatusComponent::OnRep_CurrentHP()
{
	MG_LOG_SUBOBJECT_ROLE(LogMGNet, Log, TEXT("CurrentHP: %.1f"), CurrentHP);
	OnCurrentHPChanged.Broadcast(CurrentHP);
}

void UMGStatusComponent::OnRep_MaxHP()
{
	MG_LOG_SUBOBJECT_ROLE(LogMGNet, Log, TEXT("MaxHP: %.1f"), MaxHP);
	OnMaxHPChanged.Broadcast(MaxHP);
}

void UMGStatusComponent::SetNormalSpeed(float InSpeed)
{
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		return;
	}	// 서버에서만 실행

	NormalSpeed = InSpeed;

	if (AMGPlayerCharacter* OwnerCharacter = Cast<AMGPlayerCharacter>(GetOwner()))
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void UMGStatusComponent::OnRep_NormalSpeed()
{
	AMGPlayerCharacter* OwnerCharacter = Cast<AMGPlayerCharacter>(GetOwner());
	if (!IsValid(OwnerCharacter))
	{
		return;
	}

	if (OwnerCharacter->GetCharacterMovement())
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}

}

void UMGStatusComponent::UpdateSpeed()
{
	float TotalSpeedIncrease = 0.0f;

	// 모든 활성화된 효과를 합산
	for (const FSpeedEffect& Effect : ActiveSpeedEffects)
	{
		TotalSpeedIncrease += Effect.Amount;
	}

	// NormalSpeed에 합산된 속도 증가분 적용
	SetNormalSpeed(OriginSpeed + TotalSpeedIncrease);
}

void UMGStatusComponent::OnSpeedEffectExpired(float Amount)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	// 효과 제거 Amount로 구분하여 같은 Amount의 효과 제거
	for (int32 i = 0; i < ActiveSpeedEffects.Num(); ++i)
	{
		// 부동소수점 비교, FMath::IsNearlyEqual을 사용하는 것이 안전
		if (FMath::IsNearlyEqual(ActiveSpeedEffects[i].Amount, Amount))
		{
			ActiveSpeedEffects.RemoveAt(i);
			break;
		}
	}

	for (int32 i = SpeedTimerHandler.Num() - 1; i >= 0; --i)
	{
		// 타이머가 아직 활성화 상태라면 Early continue
		if (World->GetTimerManager().IsTimerActive(SpeedTimerHandler[i]))
		{
			continue;
		}

		// 위 조건을 통과했다면 타이머가 끝난 Deactive 상태, 배열에서 삭제
		SpeedTimerHandler.RemoveAt(i);
	}

	// 속도 재계산 및 적용
	UpdateSpeed();
}

void UMGStatusComponent::AddNormalSpeedforDuration(float Amount, float Duration)
{
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		return;
	}	// 서버에서만 실행

	FSpeedEffect NewEffect;
	NewEffect.Amount = Amount;
	NewEffect.Duration = Duration;
	ActiveSpeedEffects.Add(NewEffect);

	// 총 속도를 계산해서 바로 적용
	UpdateSpeed();

	// 타이머 설정
	FTimerHandle NewTimerHandler;
	FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &UMGStatusComponent::OnSpeedEffectExpired, Amount);

	GetWorld()->GetTimerManager().SetTimer(
		NewTimerHandler,
		TimerDel,
		Duration,
		false
	);

	// 타이머 핸들을 배열에 추가
	SpeedTimerHandler.Add(NewTimerHandler);
}

void UMGStatusComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	for (FTimerHandle& TimerHandle : SpeedTimerHandler)
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
	}

	// 기존 배열 비우기
	ActiveSpeedEffects.Empty();
	SpeedTimerHandler.Empty();
}
