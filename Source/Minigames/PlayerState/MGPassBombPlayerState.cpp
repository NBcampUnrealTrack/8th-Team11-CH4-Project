// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/MGPassBombPlayerState.h"
#include "Character/MGPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/MGSpectatorPawn.h"
#include "Minigames.h"

void AMGPassBombPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// 해당 액터는 자기 자신에게만 하나 생성함.
	if (IsValid(GetPlayerController()) && GetPlayerController()->IsLocalController())
	{
		Spectator = GetWorld()->SpawnActor<AMGSpectatorPawn>(SpectatorClass);
	}
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
		if (IsValid(GetPlayerController()))
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
