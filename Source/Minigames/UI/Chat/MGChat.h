#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "Type/MGChatType.h"
#include "MGChat.generated.h"

class UEditableTextBox;
class UScrollBox;
class UMGChatLine;
/**
 *
 */
UCLASS()
class MINIGAMES_API UMGChat : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_ChatLog;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	void AddChatMessage(const FMGChatType& InChatMessage);

protected:
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMGChatLine> ChatLineClass;
};