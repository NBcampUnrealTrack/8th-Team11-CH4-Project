// MGBombActor.cpp

#include "Gimmick/MGBombActor.h"
#include "Minigames.h"						// Log 매크로 
#include "Component/MGStatusComponent.h"	// PlayerCharacter의 Status -> HP, Stamina 등 이용
#include "Components/SphereComponent.h"		// 폭탄을 넘길 Sphere Collision
#include "GameFramework/Character.h"		// Character
#include "Net/UnrealNetwork.h"				// Replication

AMGBombActor::AMGBombActor() :
	PassTriggerRadius(100.f),
	bCanPass(true),
	PassCooldownTime(0.5f)
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombStaticMesh"));
	BombMesh->SetupAttachment(RootComponent);
	BombMesh->SetCollisionProfileName(TEXT("NoCollision")); // 폭탄 자체는 NoCollision


	PassTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("PassTrigger"));
	PassTrigger->SetupAttachment(RootComponent);
	PassTrigger->SetSphereRadius(PassTriggerRadius);

	bReplicates = true;
}

void AMGBombActor::BeginPlay()
{
	Super::BeginPlay();
	
	// 서버에서만 충돌을 감지하도록 연결
	if (HasAuthority())
	{
		PassTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMGBombActor::OnTriggerOverlap);
	}
}

void AMGBombActor::OnTriggerOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, 
	const FHitResult& SweepResult)
{
	MG_LOG_ROLE(LogMGNet, Warning, TEXT(" Overlap Detected : %s"), *OtherActor->GetName());
}

void AMGBombActor::OnRep_BombHolder()
{
}

void AMGBombActor::SetBombHolder(ACharacter* NewHolder)
{
}

void AMGBombActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, BombHolder);
}

void AMGBombActor::AttachToCurrentHolder()
{
}

void AMGBombActor::ResetPassCooldown()
{
}
