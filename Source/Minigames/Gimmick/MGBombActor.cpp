// MGBombActor.cpp

#include "Gimmick/MGBombActor.h"

#include "Component/MGStatusComponent.h"	// 데미지를 주기 위해

AMGBombActor::AMGBombActor()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AMGBombActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMGBombActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

