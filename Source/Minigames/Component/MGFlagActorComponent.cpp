
#include "MGFlagActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Character/MGPlayerCharacter.h"

UMGFlagActorComponent::UMGFlagActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UMGFlagActorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMGFlagActorComponent, bFlagState);
	DOREPLIFETIME(UMGFlagActorComponent, bIsFlagProtected);
}

void UMGFlagActorComponent::RegisterFlagMeshes(UStaticMeshComponent* InFlagMesh, UStaticMeshComponent* InFlagEffectMesh)
{
	FlagMeshComp = InFlagMesh;
	FlagEffectMeshComp = InFlagEffectMesh;
}

bool UMGFlagActorComponent::SetHasFlag(bool bHasFlag)
{
	bFlagState = bHasFlag;

	if (IsValid(FlagMeshComp))
	{
		FlagMeshComp->SetVisibility(bFlagState);
	}

	AActor* Owner = GetOwner();
	if (IsValid(Owner) && Owner->HasAuthority())
	{
		if (bFlagState == true)
		{
			bIsFlagProtected = true;
			OnRep_IsFlagProtected();
			GetWorld()->GetTimerManager().SetTimer(FlagProtectTimerHandle, this, &ThisClass::ClearFlagProtection, 2.0f, false);
		}
		else
		{
			bIsFlagProtected = false;
			OnRep_IsFlagProtected();
			GetWorld()->GetTimerManager().ClearTimer(FlagProtectTimerHandle);
		}
	}

	return bFlagState;
}

void UMGFlagActorComponent::ClearFlagProtection()
{
	bIsFlagProtected = false;
	OnRep_IsFlagProtected();
}

void UMGFlagActorComponent::ServerRPCTakeFlag_Implementation()
{
	if (bFlagState == true) return;

	AMGPlayerCharacter* OwnerCharacter = Cast<AMGPlayerCharacter>(GetOwner());
	if (!IsValid(OwnerCharacter)) return;

	TArray<FHitResult> OutHitResults;
	FCollisionQueryParams Params(NAME_None, false, OwnerCharacter);

	const float StealRange = 1500.f;
	const FVector Start = OwnerCharacter->GetActorLocation();
	const FVector End = Start;

	bool bIsHitDetected = GetWorld()->SweepMultiByChannel(OutHitResults, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(StealRange), Params);

	if (bIsHitDetected == true)
	{
		for (auto const& OutHitResult : OutHitResults)
		{
			AMGPlayerCharacter* TargetCharacter = Cast<AMGPlayerCharacter>(OutHitResult.GetActor());

			if (IsValid(TargetCharacter))
			{
				// 캐릭터가 가진 깃발 컴포넌트 탐색
				UMGFlagActorComponent* TargetFlagComp = TargetCharacter->GetComponentByClass<UMGFlagActorComponent>();

				if (IsValid(TargetFlagComp) && TargetFlagComp->GetHasFlag() == true && TargetFlagComp->GetIsFlagProtected() == false)
				{
					TargetFlagComp->SetHasFlag(false);
					this->SetHasFlag(true);
					break;
				}
			}
		}
	}
}

bool UMGFlagActorComponent::ServerRPCTakeFlag_Validate()
{
	return true;
}

void UMGFlagActorComponent::OnRep_FlagState()
{
	if (IsValid(FlagMeshComp))
	{
		FlagMeshComp->SetVisibility(bFlagState);
	}
}

void UMGFlagActorComponent::OnRep_IsFlagProtected()
{
	if (IsValid(FlagEffectMeshComp))
	{
		FlagEffectMeshComp->SetVisibility(bIsFlagProtected);
	}
}