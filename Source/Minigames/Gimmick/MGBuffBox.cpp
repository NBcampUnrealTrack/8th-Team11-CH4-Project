// MGBuffBox.cpp


#include "Gimmick/MGBuffBox.h"

#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Character/MGPlayerCharacter.h"

#include "Component/MGStatusComponent.h"
#include "Data/MGEffectDataAsset.h"

AMGBuffBox::AMGBuffBox()
	: RespawnTime(20.f)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);
	TriggerBox->SetBoxExtent(FVector(40.0f, 42.0f, 30.0f));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(TriggerBox);
	Mesh->SetRelativeLocation(FVector(0.0f, -3.5f, -30.0f));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	ParticleEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleEffect"));
	ParticleEffect->SetupAttachment(TriggerBox);
	ParticleEffect->bAutoActivate = false;
}

void AMGBuffBox::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
}

void AMGBuffBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	if (!HasAuthority())
	{
		return;
	}

	AMGPlayerCharacter* OverlappingCharacter = Cast<AMGPlayerCharacter>(OtherActor);
	if (!IsValid(OverlappingCharacter))
	{
		return;
	}

	UMGStatusComponent* StatusComp = OverlappingCharacter->FindComponentByClass<UMGStatusComponent>();
	if (!IsValid(StatusComp))
	{
		return;
	}

	if (ItemEffectDataArray.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0, ItemEffectDataArray.Num() - 1);
		UMGEffectDataAsset* SelectedEffect = ItemEffectDataArray[RandomIndex];

		if (IsValid(SelectedEffect))
		{
			StatusComp->AddEffectforDuration(SelectedEffect);
		}
	}

	// 서버 + 모든 클라이언트
	Multicast_OnBoxConsumed();

	// 리스폰 타이머
	GetWorld()->GetTimerManager().SetTimer(
		RespawnTimerHandler, 
		this, 
		&ThisClass::RespawnBox, 
		RespawnTime, 
		false
	);
}

void AMGBuffBox::Multicast_OnBoxConsumed_Implementation()
{
	Mesh->SetHiddenInGame(true);
	SetActorEnableCollision(false);

	ParticleEffect->Activate(true);
}

void AMGBuffBox::RespawnBox()
{
	Multicast_RespawnBox();
}

void AMGBuffBox::Multicast_RespawnBox_Implementation()
{
	Mesh->SetHiddenInGame(false);
	SetActorEnableCollision(true);
}

void AMGBuffBox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandler);

	Super::EndPlay(EndPlayReason);
}
