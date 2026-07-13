#include "UI/Chat/MGChat.h"
#include "UI/Chat/MGChatLine.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Controller/MGPlayerController.h"

void UMGChat::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UMGChat::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
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

void UMGChat::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	UE_LOG(LogTemp, Warning, TEXT("Committed: %d, Text: %s"), (int32)CommitMethod, *Text.ToString());

	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (Text.IsEmpty() == true)
		{
			return;
		}

		APlayerController* OwningPlayerController = GetOwningPlayer();
		if (IsValid(OwningPlayerController) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("OwningPlayer is invalid!"));
			return;
		}

		AMGPlayerController* OwningMGPlayerController = Cast<AMGPlayerController>(OwningPlayerController);
		if (IsValid(OwningMGPlayerController) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cast to MGPlayerController failed!"));
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Calling AddChatMessage"));
		OwningMGPlayerController->SetChatMessageString(Text.ToString());
		EditableTextBox_ChatInput->SetText(FText());
	}
}