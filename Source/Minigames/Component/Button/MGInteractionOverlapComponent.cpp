#include "Component/Button/MGInteractionOverlapComponent.h"

#include "Components/CapsuleComponent.h"
#include "Gimmick/Button/MGInteractable.h"

void UMGInteractionOverlapComponent::BeginPlay()
{
    Super::BeginPlay();

    UCapsuleComponent* Capsule =
        GetOwner()->FindComponentByClass<UCapsuleComponent>();

    if (!Capsule)
    {
        return;
    }

    Capsule->OnComponentBeginOverlap.AddDynamic(
        this,
        &ThisClass::OnBeginOverlap);

    Capsule->OnComponentEndOverlap.AddDynamic(
        this,
        &ThisClass::OnEndOverlap);
}

void UMGInteractionOverlapComponent::OnBeginOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!OtherActor)
    {
        return;
    }

    if (OtherActor->Implements<UMGInteractable>())
    {
        IMGInteractable::Execute_BeginInteract(
            OtherActor,
            GetOwner());
    }
}

void UMGInteractionOverlapComponent::OnEndOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (!OtherActor)
    {
        return;
    }

    if (OtherActor->Implements<UMGInteractable>())
    {
        IMGInteractable::Execute_EndInteract(
            OtherActor,
            GetOwner());
    }
}