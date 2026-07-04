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
	if (IsValid(GetPlayerController()))
	{
		Spectator = GetWorld()->SpawnActor<AMGSpectatorPawn>(SpectatorClass);
	}
}

void AMGPassBombPlayerState::MulticastRPC_RetireCharacter_Implementation()
{
	AMGPlayerCharacter* MGPC = Cast<AMGPlayerCharacter>(GetPawn());

	if (IsValid(MGPC))
	{
		// 탈락 로그 출력
		MG_LOG_NET(LogMGNet, Log, TEXT("%s has retired."), *MGPC->GetName());

		// 탈락 대상자의 탈락효과 부여 (래그돌화, 폭발로 위로 튀어오름)
		MGPC->GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		MGPC->GetMesh()->SetSimulatePhysics(true);
		MGPC->GetMesh()->AddImpulse(FVector(0, 0, 10000.f),NAME_None,true);
		MGPC->GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));

		// 탈락 대상자의 컨트롤러
		if (IsValid(GetPlayerController()))
		{
			// 캐릭터의 입력을 막고 관찰자 모드로 전환
			MGPC->DisableInput(GetPlayerController());
			MG_LOG_NET(LogMGNet, Log, TEXT("SpectatorPawnName: %s"), *Spectator->GetName());
			Spectator->DeathCamFollowCharacter(GetPlayerController(), MGPC, 3.f);
		}
	}
}
