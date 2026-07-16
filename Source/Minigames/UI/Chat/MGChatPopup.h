#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Type/MGChatType.h"
#include "MGChatPopup.generated.h"

class UTextBlock;

UCLASS()
class MINIGAMES_API UMGChatPopup : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowMessage(const FMGChatType& InChatMessage);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Nickname;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Message;

	UPROPERTY(EditDefaultsOnly, Category = "Popup")
	int32 MaxPreviewLength = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Popup")
	float VisibleDuration = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Popup")
	float FadeDuration = 0.3f;

private:
	enum class EPopupState : uint8
	{
		FadeIn,
		Visible,
		FadeOut
	};

	EPopupState State = EPopupState::FadeIn;
	float StateElapsedTime = 0.0f;
};