// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/MGPassBombPlayerState.h"
#include "Character/MGPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/MGSpectatorPawn.h"
#include "Gimmick/MGBombActor.h"
#include "Net/UnrealNetwork.h"
#include "Minigames.h"

void AMGPassBombPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어만 생성
	bool bLocalControlled = GetOwningController() != nullptr && GetOwningController()->IsLocalController();
	MG_LOG_NET(LogMGNet, Log, TEXT("IsDedicatedServer: %s / LocalControlled: %s"), 
		GetNetMode() == ENetMode::NM_DedicatedServer ? TEXT("True") : TEXT("False"),
		bLocalControlled ? TEXT("True") : TEXT("False"));

	if (bLocalControlled)
	{
		ServerRPC_SetSpectator();
	}
}

void AMGPassBombPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Spectator);
}

void AMGPassBombPlayerState::ServerRPC_SetSpectator_Implementation()
{
	if (Spectator != nullptr)
	{
		return;
	}

	Spectator = GetWorld()->SpawnActor<AMGSpectatorPawn>(SpectatorClass);
}

void AMGPassBombPlayerState::MulticastRPC_RetireCharacter_Implementation()
{
	AMGPlayerCharacter* MGPC = Cast<AMGPlayerCharacter>(GetPawn());

	if (IsValid(MGPC))
	{
		const FVector ImpulseVec(0, 0, 10000.f);

		// 탈락 로그 출력
		MG_LOG_NET(LogMGNet, Log, TEXT("%s has retired."), *MGPC->GetName());

		// 탈락 대상자의 탈락효과 부여 (래그돌화, 폭발로 위로 튀어오름)
		MGPC->GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		MGPC->GetMesh()->SetSimulatePhysics(true);
		MGPC->GetMesh()->AddImpulse(ImpulseVec,NAME_None,true);
		MGPC->GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));

		// 탈락 대상자의 컨트롤러
		if (GetOwningController() != nullptr && GetOwningController()->IsLocalController())
		{
			// 캐릭터의 입력을 막고 관찰자 모드로 전환
			MGPC->DisableInput(GetPlayerController());
			if (IsValid(Spectator))
			{
				Spectator->DeathCamFollowCharacter(MGPC);
			}
		}
		else
		{
			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			if (IsValid(PC))
			{
				AMGPassBombPlayerState* MGPS = PC->GetPlayerState<AMGPassBombPlayerState>();
				if (IsValid(MGPS))
				{
					AMGSpectatorPawn* MGSP = MGPS->Spectator;
					if (IsValid(MGSP))
					{
						if (MGSP->GetFollowingMesh() == MGPC->GetMesh())
						{
							MG_LOG_NET(LogMGNet, Log, TEXT("%s: Is Calling \"SetTimerToChangeTarget\"..."), *MGSP->GetName());
							MGSP->SetTimerToChangeTarget();
						}
					}
				}
			}
		}
	}
}
