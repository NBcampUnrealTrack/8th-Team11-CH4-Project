// UW_FlagLayout.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_FlagLayout.generated.h"

class UTextBlock;

UCLASS()
class MINIGAMES_API UUW_FlagLayout : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateFlagHolderText(const FString& NewHolderName);
	void UpdateFlagTimerText(int32 RemainTime);
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FlagHolderText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FlagTimerText;
};
