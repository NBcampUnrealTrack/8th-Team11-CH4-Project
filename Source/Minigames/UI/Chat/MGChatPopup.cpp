#include "UI/Chat/MGChatPopup.h"
#include "Components/TextBlock.h"
#include "Type/MGPlayerColor.h"

void UMGChatPopup::ShowMessage(const FMGChatType& InChatMessage)
{
	if (IsValid(TextBlock_Nickname) == false || IsValid(TextBlock_Message) == false)
	{
		return;
	}

	TextBlock_Nickname->SetText(FText::FromString(FString::Printf(TEXT("%s "), *InChatMessage.SenderName)));
	TextBlock_Nickname->SetColorAndOpacity(FSlateColor(MGPlayerColorToLinear(InChatMessage.SenderColor)));

	FString Message = InChatMessage.Message;
	const int32 RemainLen = MaxPreviewLength - (InChatMessage.SenderName.Len() + 1);
	if (Message.Len() > RemainLen)
	{
		Message = Message.Left(FMath::Max(0, RemainLen)) + TEXT("...");
	}
	TextBlock_Message->SetText(FText::FromString(Message));

	SetRenderOpacity(0.0f);
	State = EPopupState::FadeIn;
	StateElapsedTime = 0.0f;
}
void UMGChatPopup::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	StateElapsedTime += InDeltaTime;

	switch (State)
	{
	case EPopupState::FadeIn:
	{
		const float Alpha = FMath::Clamp(StateElapsedTime / FadeDuration, 0.0f, 1.0f);
		SetRenderOpacity(Alpha);

		if (Alpha >= 1.0f)
		{
			State = EPopupState::Visible;
			StateElapsedTime = 0.0f;
		}
		break;
	}
	case EPopupState::Visible:
	{
		if (StateElapsedTime >= VisibleDuration)
		{
			State = EPopupState::FadeOut;
			StateElapsedTime = 0.0f;
		}
		break;
	}
	case EPopupState::FadeOut:
	{
		const float Alpha = 1.0f - FMath::Clamp(StateElapsedTime / FadeDuration, 0.0f, 1.0f);
		SetRenderOpacity(Alpha);

		if (Alpha <= 0.0f)
		{
			RemoveFromParent(); 
		}
		break;
	}
	}
}