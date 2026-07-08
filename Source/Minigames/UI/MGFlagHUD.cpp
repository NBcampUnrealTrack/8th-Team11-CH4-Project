// MGFlagHUD.cpp

#include "UI/MGFlagHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/UW_FlagLayout.h"
#include "GameState/MGFlagGameStateBase.h"
#include "PlayerState/MGFlagPlayerState.h"

void AMGFlagHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(FlagWidgetClass))
	{
		return;
	}

	FlagWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), FlagWidgetClass);
	if (IsValid(FlagWidgetInstance))
	{
		FlagWidgetInstance->AddToViewport();
	}
	
	if (AMGFlagGameStateBase* FGS = GetWorld()->GetGameState<AMGFlagGameStateBase>())
	{
		BindWithGameState(FGS);
	}
}

void AMGFlagHUD::BindWithGameState(AMGFlagGameStateBase* FGS)
{
	if (!IsValid(FGS))
	{
		return;
	}
	
	FGS->OnRemainTimeChanged.AddUObject(this, &AMGFlagHUD::OnRemainTimeUpdated);
	FGS->OnFlagHolderChanged.AddUObject(this, &AMGFlagHUD::OnFlagHolderUpdated);

	// 구독 직후 현재값으로 즉시 1회 갱신
	OnRemainTimeUpdated(FGS->RemainGameTime);
	OnFlagHolderUpdated(FGS->GetCurrentFlagHolder());
}

void AMGFlagHUD::OnRemainTimeUpdated(int32 RemainTime)
{
	if (UUW_FlagLayout* MyWidget = Cast<UUW_FlagLayout>(FlagWidgetInstance))
	{
		MyWidget->UpdateFlagTimerText(RemainTime);
	}
}

void AMGFlagHUD::OnFlagHolderUpdated(AMGFlagPlayerState* NewHolder)
{
	UUW_FlagLayout* MyWidget = Cast<UUW_FlagLayout>(FlagWidgetInstance);
	if (!IsValid(MyWidget))
	{
		return;
	}

	FString HolderName = IsValid(NewHolder) ? NewHolder->GetPlayerName() : TEXT("없음");
	MyWidget->UpdateFlagHolderText(HolderName);
}