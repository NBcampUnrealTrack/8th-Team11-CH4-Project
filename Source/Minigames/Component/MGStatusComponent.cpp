// MGStatusComponent.cpp


#include "Component/MGStatusComponent.h"

#include "Net/UnrealNetwork.h"
#include "Minigames.h"				// 매크로 사용

#include "Character/MGPlayerCharacter.h"				// 이동속도
#include "GameFramework/CharacterMovementComponent.h"	// 이동속도

#include "Data/MGEffectDataAsset.h"						// DataAsset

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

#pragma region MovementSpeed

void UMGStatusComponent::AddEffectforDuration(const UMGEffectDataAsset* InEffectData)
{
	if (!InEffectData || !IsValid(GetOwner()) || !GetOwner()->HasAuthority())
	{
		return;
	}	// 유효성 검사 & 서버 권한 검사

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}	// 유효성 검사

	if (!InEffectData->bCanStack)
	{
		for (FSpeedEffect& Effect : ActiveSpeedEffects)
		{
			// 동일한 데이터 에셋인지 확인
			if (Effect.BuffData == InEffectData)
			{
				World->GetTimerManager().ClearTimer(Effect.TimerHandle);

				// 기존 버프의 타이머만 다시 최대치로 갱신 (Refresh)
				FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &UMGStatusComponent::OnSpeedEffectExpired, Effect.UniqueID);
				World->GetTimerManager().SetTimer(Effect.TimerHandle, TimerDel, InEffectData->Duration, false);

				return; // 새로 추가하지 않고 여기서 끝냅니다.
			}
		}
	}

	// 3. 중첩 가능이거나 완전히 새로운 버프인 경우
	FSpeedEffect NewEffect;
	NewEffect.UniqueID = FGuid::NewGuid(); // 고유 ID 발급
	NewEffect.BuffData = InEffectData;     // 오타 수정: InBuffData -> InEffectData

	FTimerDelegate TimerDel = 
		FTimerDelegate::CreateUObject(this, &UMGStatusComponent::OnSpeedEffectExpired, NewEffect.UniqueID);
	
	World->GetTimerManager().SetTimer(
		NewEffect.TimerHandle, 
		TimerDel, 
		InEffectData->Duration, 
		false);

	// 버프 목록에 추가
	ActiveSpeedEffects.Add(NewEffect);

	// 속도를 재계산
	UpdateSpeed();
}

void UMGStatusComponent::UpdateSpeed()
{
	float TotalSpeedIncrease = 0.0f;

	// 모든 활성화된 효과를 합산
	for (const FSpeedEffect& Effect : ActiveSpeedEffects)
	{
		// 데이터 에셋에서 수치를 가져옴
		if (IsValid(Effect.BuffData))
		{
			TotalSpeedIncrease += Effect.BuffData->SpeedAmount;
		}
	}

	// 합산된 속도 적용
	SetNormalSpeed(OriginSpeed + TotalSpeedIncrease);
}

void UMGStatusComponent::SetNormalSpeed(float InSpeed)
{
	if (IsValid(GetOwner()) == false || GetOwner()->HasAuthority() == false)
	{
		return;
	}	// 유효성 검사 && 서버 권한 확인

	// 값이 변경되면 ReplicatedUsing = OnRep_NormalSpeed
	NormalSpeed = InSpeed;

	// OnRep_NormalSpeed에서 처리할 내용을 서버의 실제 이동 속도에도 적용
	if (AMGPlayerCharacter* OwnerCharacter = Cast<AMGPlayerCharacter>(GetOwner()))
	{
		if (OwnerCharacter->GetCharacterMovement())
		{
			OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
		}
	}
}

void UMGStatusComponent::OnRep_NormalSpeed()
{
	// Client의 AutonomousProxy Character
	AMGPlayerCharacter* OwnerCharacter = Cast<AMGPlayerCharacter>(GetOwner());
	if (!IsValid(OwnerCharacter))
	{
		return;
	}

	// Character 속도 변경
	if (OwnerCharacter->GetCharacterMovement())
	{
		OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void UMGStatusComponent::OnSpeedEffectExpired(FGuid ExpiredEffectID)
{
	// 고유 ID ExpiredEffectID로 해당 버프를 찾아서 제거
	// TODO : 현재 TArray이기 때문에 순회 로직이 필요함 -> TMap으로 변경 필요
	for (auto It = ActiveSpeedEffects.CreateIterator(); It; ++It)
	{
		if (It->UniqueID == ExpiredEffectID)
		{
			It.RemoveCurrent(); // 현재 Iterator가 가리키고 있는 요소 삭제
			break;              // 찾아서 지웠으면 반복문 즉시 종료
		}
	}

	// Effect 하나가 사라졌으니 속도 다시 계산
	UpdateSpeed();
}

#pragma endregion

void UMGStatusComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	// 존재하는 모든 타이머를 모두 취소
	for (FSpeedEffect& Effect : ActiveSpeedEffects)
	{
		World->GetTimerManager().ClearTimer(Effect.TimerHandle);
	}

	ActiveSpeedEffects.Empty();
}
