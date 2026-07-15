// MGTitlePlayerController.cpp


#include "Controller/MGTitlePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/MGOnlineSubsystem.h"
#include "Engine/GameInstance.h"
#include "MGNetConfig.h"

void AMGTitlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}
	
#if MG_USE_EOS
	if (UMGOnlineSubsystem* OnlineSub = GetGameInstance()->GetSubsystem<UMGOnlineSubsystem>())
	{
		OnlineSub->Login();
	}
#endif

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
#if MG_USE_EOS
	if (UMGOnlineSubsystem* Online = GetGameInstance()->GetSubsystem<UMGOnlineSubsystem>())
	{
		Online->HostSession();
	}
#else
	GetWorld()->ServerTravel(TEXT("/Game/Minigames/Level/L_Lobby?listen"));
#endif
}

void AMGTitlePlayerController::JoinGame()
{
#if MG_USE_EOS
	if (UMGOnlineSubsystem* Online = GetGameInstance()->GetSubsystem<UMGOnlineSubsystem>())
	{
		Online->FindAndJoinSession();
	}
#else
	ClientTravel(MG_DIRECT_IP, TRAVEL_Absolute);
#endif
}
