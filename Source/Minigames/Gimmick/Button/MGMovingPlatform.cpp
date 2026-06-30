// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Button/MGMovingPlatform.h"

// Sets default values
AMGMovingPlatform::AMGMovingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMGMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMGMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

