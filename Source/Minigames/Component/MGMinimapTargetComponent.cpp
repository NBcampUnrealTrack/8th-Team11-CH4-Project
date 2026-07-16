// MGMinimapTargetComponent.cpp

#include "Component/MGMinimapTargetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "UI/MGHUDBase.h"
#include "UI/MiniMap/UW_MiniMapLayout.h"
#include "GameState/MGGameStateBase.h"

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

	GetWorld()->GetTimerManager().SetTimer(
		BindTimerHandler,
		this, 
		&UMGMinimapTargetComponent::TryBindMinigameStart,
		0.1f, 
		true
	);
}

void UMGMinimapTargetComponent::TryBindMinigameStart()
{
	AMGGameStateBase* MGGameState = GetWorld() ? GetWorld()->GetGameState<AMGGameStateBase>() : nullptr;
	if (IsValid(MGGameState))
	{
		MGGameState->OnMinigameStarted.AddDynamic(this, &UMGMinimapTargetComponent::RegisterToMinimap);
		GetWorld()->GetTimerManager().ClearTimer(BindTimerHandler);	// 바인딩에 성공하면 ClearTimer
	}
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

void UMGMinimapTargetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(BindTimerHandler);		// 타이머 정리

	Super::EndPlay(EndPlayReason);
}
