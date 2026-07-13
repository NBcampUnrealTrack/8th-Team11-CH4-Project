
#include "MGFlagActorComponent.h"
#include "Engine/OverlapResult.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/MGPlayerCharacter.h"
#include "GameState/MGFlagGameStateBase.h"
#include "PlayerState/MGFlagPlayerState.h"
#include "Gimmick/MGFlagActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PlayerState/MGPlayerState.h"

UMGFlagActorComponent::UMGFlagActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UMGFlagActorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMGFlagActorComponent, bFlagState);
	DOREPLIFETIME(UMGFlagActorComponent, ReplicatedFlagColor);
	DOREPLIFETIME(UMGFlagActorComponent, bIsFlagProtected);
}

void UMGFlagActorComponent::RegisterFlagMeshes(class UStaticMeshComponent* InFlagMesh, class UStaticMeshComponent* InFlagEffectMesh, class UNiagaraComponent* InFlagNiagara)
{
	FlagMeshComp = InFlagMesh;
	FlagEffectMeshComp = InFlagEffectMesh;
	FlagNiagaraComp = InFlagNiagara;
}

bool UMGFlagActorComponent::SetHasFlag(bool bHasFlag, AMGFlagActor* InFlagActor)
{
	AActor* Owner = GetOwner();

	if (IsValid(Owner) && Owner->HasAuthority())
	{
		if (bHasFlag)
		{
			if (AMGPlayerCharacter* PC = Cast<AMGPlayerCharacter>(Owner))
			{
				if (AMGPlayerState* PS = PC->GetPlayerState<AMGPlayerState>())
				{
					ReplicatedFlagColor = PS->GetPlayerLinearColor();
				}
			}
		}
		else
		{
			ReplicatedFlagColor = FLinearColor::White;
		}

		bFlagState = bHasFlag;

		if (IsValid(FlagMeshComp))
		{
			OnRep_FlagVisuals();
		}	
	}

	if (IsValid(Owner) && Owner->HasAuthority())
	{
		if (bFlagState == true)
		{
			bIsFlagProtected = true;
			OnRep_IsFlagProtected();
			GetWorld()->GetTimerManager().SetTimer(FlagProtectTimerHandle, this, &ThisClass::ClearFlagProtection, 2.0f, false);
		
			if (AMGFlagGameStateBase* FGS = GetWorld()->GetGameState<AMGFlagGameStateBase>())
			{
				if (AMGPlayerCharacter* OwnerCharacter = Cast<AMGPlayerCharacter>(GetOwner()))
				{
					FGS->SetCurrentFlagHolder(OwnerCharacter->GetPlayerState<AMGFlagPlayerState>());
				}
			}
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

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams Params(NAME_None, false, OwnerCharacter);

	const FVector CheckLocation = OwnerCharacter->GetActorLocation();

	bool bIsHitDetected = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		CheckLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(StealRange),
		Params
	);
	
	if (bIsHitDetected == true)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			AMGPlayerCharacter* TargetCharacter = Cast<AMGPlayerCharacter>(OverlapResult.GetActor());

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

/*
void UMGFlagActorComponent::OnRep_FlagState()
{
	if (IsValid(FlagMeshComp))
	{
		FlagMeshComp->SetVisibility(bFlagState);
	}

	OnFlagStateChanged.Broadcast(bFlagState);
}
*/

void UMGFlagActorComponent::OnRep_IsFlagProtected()
{
	if (IsValid(FlagEffectMeshComp))
	{
		FlagEffectMeshComp->SetVisibility(bIsFlagProtected);
	}

	OnFlagProtectionChanged.Broadcast(bIsFlagProtected);
}

void UMGFlagActorComponent::OnRep_FlagVisuals()
{
	if (IsValid(FlagMeshComp))
	{
		if (bFlagState == true)
		{		
			FlagMeshComp->SetVisibility(true);

			UMaterialInstanceDynamic* MID = FlagMeshComp->CreateAndSetMaterialInstanceDynamic(0);
			if (IsValid(MID))
			{
				MID->SetVectorParameterValue(FName("BaseColorFactor"), ReplicatedFlagColor);
			}
		}
		else
		{
			FlagMeshComp->SetVisibility(false);
		}
	}
	

	if (IsValid(FlagNiagaraComp))
	{
		if (bFlagState)
		{
			FlagNiagaraComp->Activate(true);
		}
		else
		{
			FlagNiagaraComp->Deactivate();
		}
	}

	OnFlagStateChanged.Broadcast(bFlagState);
}