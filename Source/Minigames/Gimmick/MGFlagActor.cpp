#include "MGFlagActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/MGPlayerCharacter.h"
#include "Component/MGFlagActorComponent.h"

AMGFlagActor::AMGFlagActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Replication
	bReplicates = true;

	// SphereComponent
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
	SphereComp->InitSphereRadius(100.f);
	// Overlap Event
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// 스태틱 메시 컴포넌트
	FlagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
	FlagMesh->SetupAttachment(RootComponent);
	// 콜리전 제거
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMGFlagActor::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서만 오버랩 이벤트
	if (HasAuthority())
	{
		SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AMGFlagActor::OnSphereOverlap);
	}
}

void AMGFlagActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	AMGPlayerCharacter* PlayerCharacter = Cast<AMGPlayerCharacter>(OtherActor);

	if (PlayerCharacter)
	{
		UMGFlagActorComponent* FlagComp = PlayerCharacter->GetComponentByClass<UMGFlagActorComponent>();
		if (FlagComp)
		{
			FlagComp->SetHasFlag(true, this);

			// 서버에서 삭제
			Destroy();
		}
	}
}