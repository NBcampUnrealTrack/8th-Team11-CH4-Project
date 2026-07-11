// MGTitlePlayerController.cpp


#include "Controller/MGTitlePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/MGOnlineSubsystem.h"
#include "Engine/GameInstance.h"

void AMGTitlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}
	
	if (UMGOnlineSubsystem* OnlineSub = GetGameInstance()->GetSubsystem<UMGOnlineSubsystem>())
	{
		OnlineSub->Login();
	}

	if (IsValid(UIWidgetClass) == true)
	{
		UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
		if (IsValid(UIWidgetInstance) == true)
		{
			UIWidgetInstance->AddToViewport();

			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
			SetInputMode(Mode);

			bShowMouseCursor = true;
		}
	}
}

void AMGTitlePlayerController::HostGame()
{
	if (UMGOnlineSubsystem* Online = GetGameInstance()->GetSubsystem<UMGOnlineSubsystem>())
	{
		Online->HostSession();
	}
}

void AMGTitlePlayerController::JoinGame()
{
	if (UMGOnlineSubsystem* Online = GetGameInstance()->GetSubsystem<UMGOnlineSubsystem>())
	{
		Online->FindAndJoinSession();
	}
}
