// MGOverlapChecker.cpp

#include "Gimmick/MGOverlapChecker.h"
#include "Components/BoxComponent.h"
#include "Character/MGPlayerCharacter.h"
#include "Component/MGStatusComponent.h"
#include "Data/MGEffectDataAsset.h"

AMGOverlapChecker::AMGOverlapChecker()
{
	PrimaryActorTick.bCanEverTick = false;

	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	RootComponent = OverlapBox;

	OverlapBox->SetCollisionProfileName(TEXT("Trigger"));
}

void AMGOverlapChecker::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(
		OverlapTimerHandler,
		this,
		&AMGOverlapChecker::CheckOverlappingActors,
		0.5f,
		true
	);
}

void AMGOverlapChecker::CheckOverlappingActors()
{
	if (!OverlapBox) return;

	CurrentOverlappingActors.Reset();

	// 현재 Box Collision에 오버랩된 MGPlayerCharacter를 모두 가져오기
	OverlapBox->GetOverlappingActors(CurrentOverlappingActors, AMGPlayerCharacter::StaticClass());

	for (AActor* Actor : CurrentOverlappingActors)
	{
		if (Actor)
		{
			AMGPlayerCharacter* OverlappingCharacter = Cast<AMGPlayerCharacter>(Actor);
			if (!IsValid(OverlappingCharacter))
			{
				return;
			}

			UMGStatusComponent* StatusComp = OverlappingCharacter->FindComponentByClass<UMGStatusComponent>();
			if (!IsValid(StatusComp))
			{
				return;
			}

			if (IsValid(ItemEffectData))
			{
				StatusComp->AddEffectforDuration(ItemEffectData);
			}
		}
	}
}

void AMGOverlapChecker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(OverlapTimerHandler);

	// 상속받은 EndPlay의 Super는 마지막에
	Super::EndPlay(EndPlayReason);
}
