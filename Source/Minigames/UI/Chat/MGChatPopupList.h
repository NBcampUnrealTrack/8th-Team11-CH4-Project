#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Type/MGChatType.h"
#include "MGChatPopupList.generated.h"

class UVerticalBox;
class UMGChatPopup;

UCLASS()
class MINIGAMES_API UMGChatPopupList : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddPopup(const FMGChatType& InChatMessage);
	void ClearAllPopups();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_Popups;

	UPROPERTY(EditDefaultsOnly, Category = "Popup")
	TSubclassOf<UMGChatPopup> PopupClass;

	UPROPERTY(EditDefaultsOnly, Category = "Popup")
	int32 MaxPopupCount = 3;
};