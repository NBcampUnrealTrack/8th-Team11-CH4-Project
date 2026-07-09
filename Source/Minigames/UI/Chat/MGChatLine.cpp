#include "UI/Chat/MGChatLine.h"

#include "Components/TextBlock.h"

void UMGChatLine::SetMessage(const FString& InMessage)
{
	if (IsValid(TextBlock_Message) == true)
	{
		TextBlock_Message->SetText(FText::FromString(InMessage));
	}
}