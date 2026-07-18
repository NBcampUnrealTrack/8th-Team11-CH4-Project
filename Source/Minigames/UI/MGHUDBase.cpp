// MGHUDBase.cpp

#include "UI/MGHUDBase.h"
#include "UI/MiniMap/UW_MiniMapLayout.h"
#include "Minigames.h"
#include "GameState/MGGameStateBase.h"

void AMGHUDBase::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}	// Dedicated Server는 조기 return

	GetWorld()->GetTimerManager().SetTimer(
		BindTimerHandler,
		this,
		&AMGHUDBase::TryBindGameStateDelegate,
		0.1f,
		true // 성공할 때까지 반복
	);
}

void AMGHUDBase::TryBindGameStateDelegate()
{
	AMGGameStateBase* MGGameState = GetWorld() ? GetWorld()->GetGameState<AMGGameStateBase>() : nullptr;
	if (IsValid(MGGameState))
	{
		// Delegate Broadcast를 전달받지 못했음에도 이미 상태가 바뀌었으면
		if (MGGameState->MatchState == EMatchState::Playing)
		{
			InitializeMinimap();	// 즉시 실행
		}
		else
		{	// 아직 Delegate Broadcast가 실행되지 않았고, 클래스가 정상적으로 존재할 때 Delegate Binding
			MGGameState->OnMinigameStarted.AddDynamic(this, &AMGHUDBase::InitializeMinimap);
		}
		// 어떤식으로든 처리 완료하면 Try 타이머 종료
		GetWorld()->GetTimerManager().ClearTimer(BindTimerHandler);
	}
}

void AMGHUDBase::InitializeMinimap()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}	// 혹시 Dedicated Server에서 호출된다면 서버는 조기 return

	if (!IsValid(MinimapWidgetClass))
	{
		return;
	}

	MinimapWidget = CreateWidget<UUW_MiniMapLayout>(GetOwningPlayerController(), MinimapWidgetClass);
	if (IsValid(MinimapWidget))
	{
		MinimapWidget->AddToViewport();
	}
	else
	{
		MG_LOG_ROLE(LogMGNet, Log, TEXT("Fail to add Widget to Viewport"));
	}
}


void AMGHUDBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(BindTimerHandler);
}
