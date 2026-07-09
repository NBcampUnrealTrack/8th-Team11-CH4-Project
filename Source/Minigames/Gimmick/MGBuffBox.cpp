// MGBuffBox.cpp


#include "Gimmick/MGBuffBox.h"

#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Character/MGPlayerCharacter.h"

#include "Component/MGStatusComponent.h"
#include "Data/MGEffectDataAsset.h"

AMGBuffBox::AMGBuffBox()
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
	ParticleEffect->Activate(true);
	Mesh->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	ParticleEffect->OnSystemFinished.AddDynamic(this, &ThisClass::OnEffectFinished);

	AMGPlayerCharacter* OverlappingCharacter = Cast<AMGPlayerCharacter>(OtherActor);
	UMGStatusComponent* StatusComp = OverlappingCharacter->FindComponentByClass<UMGStatusComponent>();

	if (!IsValid(OverlappingCharacter))
	{
		return;
	}

	if (!IsValid(StatusComp))
	{
		return;
	}

	if (IsValid(ItemEffectData))
	{
		StatusComp->AddEffectforDuration(ItemEffectData);
	}
}

void AMGBuffBox::OnEffectFinished(UParticleSystemComponent* ParticleSystem)
{
	if (HasAuthority() == true)
	{
		Destroy();
	}
}
