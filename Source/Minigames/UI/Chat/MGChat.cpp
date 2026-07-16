#include "UI/Chat/MGChat.h"
#include "UI/Chat/MGChatLine.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Controller/MGPlayerController.h"
#include "GameInstance/MGGameInstance.h"

void UMGChat::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
	if (IsValid(Button_Send) == true
		&& Button_Send->OnClicked.IsAlreadyBound(this, &ThisClass::OnSendButtonClicked) == false)
	{
		Button_Send->OnClicked.AddDynamic(this, &ThisClass::OnSendButtonClicked);
	}
}

void UMGChat::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
	if (IsValid(Button_Send) == true
		&& Button_Send->OnClicked.IsAlreadyBound(this, &ThisClass::OnSendButtonClicked) == true)
	{
		Button_Send->OnClicked.RemoveDynamic(this, &ThisClass::OnSendButtonClicked);
	}
}

void UMGChat::AddChatMessage(const FMGChatType& InChatMessage)
{
	if (IsValid(ScrollBox_ChatLog) == false || IsValid(ChatLineClass) == false)
	{
		return;
	}

	UMGChatLine* NewChatLine = CreateWidget<UMGChatLine>(this, ChatLineClass);
	if (IsValid(NewChatLine) == true)
	{
		NewChatLine->SetMessage(InChatMessage);
		ScrollBox_ChatLog->AddChild(NewChatLine);
		ScrollBox_ChatLog->ScrollToEnd();
	}
}

void UMGChat::FocusChatInput()
{
	if (IsValid(EditableTextBox_ChatInput) == true)
	{
		EditableTextBox_ChatInput->SetKeyboardFocus();
	}
}

void UMGChat::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnUserMovedFocus)
	{
		if (IsValid(Button_Send) == true && Button_Send->IsHovered() == true)
		{
			return;
		}

		if (AMGPlayerController* MGPC = Cast<AMGPlayerController>(GetOwningPlayer()))
		{
			MGPC->RestoreDefaultInputMode();
		}
		return;
	}

	if (CommitMethod != ETextCommit::OnEnter)
	{
		return;
	}

	if (Text.IsEmpty() == true)
	{
		SetVisibility(ESlateVisibility::Collapsed);

		if (UMGGameInstance* GI = GetGameInstance<UMGGameInstance>())
		{
			GI->bChatVisible = false;
		}

		if (AMGPlayerController* MGPC = Cast<AMGPlayerController>(GetOwningPlayer()))
		{
			MGPC->RestoreDefaultInputMode();
		}
		return;
	}

	SendChatMessage();
}

void UMGChat::OnSendButtonClicked()
{
	SendChatMessage();
	EditableTextBox_ChatInput->SetKeyboardFocus();
}

void UMGChat::SendChatMessage()
{
	if (IsValid(EditableTextBox_ChatInput) == false)
	{
		return;
	}

	const FText Text = EditableTextBox_ChatInput->GetText();
	if (Text.IsEmpty() == true)
	{
		return;
	}

	AMGPlayerController* OwningMGPlayerController = Cast<AMGPlayerController>(GetOwningPlayer());
	if (IsValid(OwningMGPlayerController) == false)
	{
		return;
	}

	OwningMGPlayerController->SetChatMessageString(Text.ToString());
	EditableTextBox_ChatInput->SetText(FText());
}