// MGMinimapTargetComponent.cpp

#include "Component/MGMinimapTargetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "UI/MGHUDBase.h"
#include "UI/MiniMap/UW_MiniMapLayout.h"

UMGMinimapTargetComponent::UMGMinimapTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMGMinimapTargetComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	FTimerHandle RegisterTimerHandler;
	GetWorld()->GetTimerManager().SetTimer(
		RegisterTimerHandler, 
		this, 
		&UMGMinimapTargetComponent::RegisterToMinimap, 
		1.0f, 
		false
	);
}

void UMGMinimapTargetComponent::RegisterToMinimap()
{
	AActor* OwnerActor = GetOwner();
	if (!IsValid(OwnerActor))
	{
		return;
	}

	APlayerController* LocalPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(LocalPC))
	{
		return;
	}

	AMGHUDBase* MyHUD = Cast<AMGHUDBase>(LocalPC->GetHUD());
	if (!IsValid(MyHUD))
	{
		return;
	}

	if (!IsValid(MyHUD->MinimapWidget))
	{
		return;
	}

	// 일단은 기본 Type::None으로 지정, 추후에 지정한 Type으로 Override
	EMinimapTargetType FinalType = TargetType;
	APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	if (IsValid(OwnerPawn) && OwnerPawn->IsLocallyControlled())
	{
		FinalType = EMinimapTargetType::My;		// 자신이 조종하고 있는 Pawn은 Type::My로
	}

	MyHUD->MinimapWidget->AddMinimapTarget(OwnerActor, FinalType);	// 미니맵에 등록
}

