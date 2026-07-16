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

	AMGGameStateBase* MGGameState = GetWorld()->GetGameState<AMGGameStateBase>();
	if (ensure(IsValid(MGGameState)) == false)
	{
		return;
	}
	MGGameState->OnMinigameStarted.AddDynamic(this, &AMGHUDBase::InitializeMinimap);
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
