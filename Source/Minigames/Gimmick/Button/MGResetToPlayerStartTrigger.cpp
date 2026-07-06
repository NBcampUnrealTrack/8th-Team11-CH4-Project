#include "MGResetToPlayerStartTrigger.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "TimerManager.h"

AMGResetToPlayerStartTrigger::AMGResetToPlayerStartTrigger()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = false;

    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    RootComponent = TriggerVolume;

    TriggerVolume->SetBoxExtent(FVector(100.f, 100.f, 100.f));
    TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerVolume->SetGenerateOverlapEvents(true);
}

void AMGResetToPlayerStartTrigger::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AMGResetToPlayerStartTrigger::OnTriggerOverlapBegin);
    }
}

APlayerStart* AMGResetToPlayerStartTrigger::FindTargetPlayerStart() const
{
    if (TargetPlayerStart)
    {
        return TargetPlayerStart;
    }

    if (!PlayerStartTag.IsNone())
    {
        for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
        {
            if (It->PlayerStartTag == PlayerStartTag)
            {
                return *It;
            }
        }
    }

    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        return *It;
    }

    return nullptr;
}

void AMGResetToPlayerStartTrigger::OnTriggerOverlapBegin(
    UPrimitiveComponent* OverlappedComponent,
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

    ACharacter* Character = Cast<ACharacter>(OtherActor);
    if (!Character)
    {
        return;
    }

    const float Now = GetWorld()->GetTimeSeconds();
    if (const float* LastTime = LastResetTimeMap.Find(Character))
    {
        if (Now - *LastTime < ResetCooldownSeconds)
        {
            return;
        }
    }

    APlayerStart* StartPoint = FindTargetPlayerStart();
    if (!StartPoint)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] PlayerStart를 찾지 못해 리셋을 수행할 수 없습니다."), *GetName());
        return;
    }

    LastResetTimeMap.Add(Character, Now);

    Character->GetCharacterMovement()->StopMovementImmediately();
    Character->GetCharacterMovement()->SetBase(nullptr);

    const FVector TargetLocation = StartPoint->GetActorLocation();
    const FRotator TargetRotation = StartPoint->GetActorRotation();

    Character->TeleportTo(TargetLocation, TargetRotation, false, true);

    Character->ForceNetUpdate();

    if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
    {
        PC->ClientSetRotation(TargetRotation);
    }
}