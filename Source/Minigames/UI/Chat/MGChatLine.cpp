#include "UI/Chat/MGChatLine.h"

#include "Components/TextBlock.h"
#include "Type/MGPlayerColor.h"

void UMGChatLine::SetMessage(const FMGChatType& InChatMessage)
{
	if (IsValid(TextBlock_Nickname) == true)
	{
		TextBlock_Nickname->SetText(FText::FromString(FString::Printf(TEXT("%s:"), *InChatMessage.SenderName)));
		TextBlock_Nickname->SetColorAndOpacity(FSlateColor(MGPlayerColorToLinear(InChatMessage.SenderColor)));
	}

	if (IsValid(TextBlock_Message) == true)
	{
		TextBlock_Message->SetText(FText::FromString(InChatMessage.Message));
	}
}