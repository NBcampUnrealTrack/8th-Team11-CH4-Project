// UW_TitleLayout.cpp


#include "UI/UW_TitleLayout.h"

#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Controller/MGTitlePlayerController.h"
#include "GameInstance/MGGameInstance.h"

UUW_TitleLayout::UUW_TitleLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUW_TitleLayout::NativeConstruct()
{
	PlayButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	ExitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
	JoinButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnJoinButtonClicked);

	TitleSwitcher->SetActiveWidgetIndex(0);
}

void UUW_TitleLayout::OnPlayButtonClicked()
{
	TitleSwitcher->SetActiveWidgetIndex(1);
}

void UUW_TitleLayout::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UUW_TitleLayout::OnJoinButtonClicked()
{
	AMGTitlePlayerController* PlayerController = GetOwningPlayer<AMGTitlePlayerController>();
	if (IsValid(PlayerController) == true)
	{
		const FString Nickname = NicknameEditableText->GetText().ToString();
		if (Nickname.IsEmpty() == true)
		{
			return;
		}
		
		UMGGameInstance* GI = GetGameInstance<UMGGameInstance>();
		if (IsValid(GI) == true)
		{
			GI->PlayerNickname = Nickname;
		}
		
		FText ServerIP = ServerIPEditableText->GetText();
		PlayerController->JoinServer(ServerIP.ToString());
	}
}
