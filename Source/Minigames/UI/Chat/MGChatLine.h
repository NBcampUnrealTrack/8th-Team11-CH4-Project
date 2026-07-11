#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Type/MGChatType.h"
#include "MGChatLine.generated.h"

class UTextBlock;

/**
 *
 */
UCLASS()
class MINIGAMES_API UMGChatLine : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMessage(const FMGChatType& InChatMessage);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Nickname;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Message;
};