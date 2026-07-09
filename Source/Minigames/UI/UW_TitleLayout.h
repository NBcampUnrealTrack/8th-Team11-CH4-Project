// UW_TitleLayout.h

#pragma once

#include "Blueprint/UserWidget.h"
#include "UW_TitleLayout.generated.h"

class UWidgetSwitcher;
class UButton;
class UEditableText;
class UEditableTextBox;

/**
 *
 */
UCLASS()
class MINIGAMES_API UUW_TitleLayout : public UUserWidget
{
	GENERATED_BODY()

public:
	UUW_TitleLayout(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnPlayButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();
	
	UFUNCTION()
	void OnJoinButtonClicked();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = USTitleWidget, Meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> PlayButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = USTitleWidget, Meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> ExitButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = USLobbyLevelUI, Meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UEditableText> ServerIPEditableText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = USTitleWidget, Meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UWidgetSwitcher> TitleSwitcher;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = USTitleWidget, Meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UEditableTextBox> NicknameEditableText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = USTitleWidget, Meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> JoinButton;

};
