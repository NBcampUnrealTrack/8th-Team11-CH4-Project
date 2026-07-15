#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "Type/MGChatType.h"
#include "MGChat.generated.h"

class UEditableTextBox;
class UScrollBox;
class UButton;
class UMGChatLine;
/**
 *
 */
UCLASS()
class MINIGAMES_API UMGChat : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddChatMessage(const FMGChatType& InChatMessage);
	void FocusChatInput();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnSendButtonClicked();

	void SendChatMessage();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_ChatLog;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Send;

	UPROPERTY(EditDefaultsOnly, Category = "Chat")
	TSubclassOf<UMGChatLine> ChatLineClass;
};