// MGMinimapTargetComponent.cpp

#include "Component/MGMinimapTargetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "UI/MGHUDBase.h"
#include "UI/MiniMap/UW_MiniMapLayout.h"
#include "GameState/MGGameStateBase.h"
#include "GameInstance/MGGameInstance.h"

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
		// 늦게 스폰되어서 이미 게임이 시작(Playing)된 경우
		if (MGGameState->MatchState == EMatchState::Playing)
		{
			RegisterToMinimap();
		}
		else
		{
			// 아직 시작 전이면 정상적으로 델리게이트 Broadcast를 기다림
			MGGameState->OnMinigameStarted.AddDynamic(this, &UMGMinimapTargetComponent::RegisterToMinimap);
		}

		// 미니맵 등록을 어떤식으로든 처리 완료하면 탐색 타이머 종료
		GetWorld()->GetTimerManager().ClearTimer(BindTimerHandler);
	}
}

void UMGMinimapTargetComponent::RegisterToMinimap()
{
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains(TEXT("FinalResult")))
	{
		return;
	}

	AActor* OwnerActor = GetOwner();
	if (!IsValid(OwnerActor))
	{
		return;
	}

	APlayerController* LocalPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(LocalPC))
	{
		GetWorld()->GetTimerManager().SetTimer(
			RetryTimerHandler, 
			this, 
			&UMGMinimapTargetComponent::RegisterToMinimap, 
			0.1f, 
			false
		);	// 자기 자신을 재호출
		return;
	}

	AMGHUDBase* MyHUD = Cast<AMGHUDBase>(LocalPC->GetHUD());
	if (!IsValid(MyHUD) || !IsValid(MyHUD->MinimapWidget))
	{
		GetWorld()->GetTimerManager().SetTimer(
			RetryTimerHandler, 
			this, 
			&UMGMinimapTargetComponent::RegisterToMinimap, 
			0.1f, 
			false
		);	// 자기 자신을 재호출
		return;
	}

	// 일단은 기본 Type::Enemy로 지정, 추후에 지정한 Type으로 Override
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
	// 타이머 정리
	GetWorld()->GetTimerManager().ClearTimer(BindTimerHandler);
	GetWorld()->GetTimerManager().ClearTimer(RetryTimerHandler);

	Super::EndPlay(EndPlayReason);
}
