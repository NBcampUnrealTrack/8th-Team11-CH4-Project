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

	UFUNCTION(BlueprintImplementableEvent, Category = "FlagUI")
	void UpdateLeaderboardUI(const TArray<class AMGFlagPlayerState*>& FlagPlayerStates);

	UFUNCTION(BlueprintImplementableEvent, Category = "FlagUI")
	void UpdateFlagStateUI(bool bHasFlag, bool bIsProtected);

	UFUNCTION(BlueprintImplementableEvent, Category = "FlagUI")
	void UpdateStealPromptUI(bool bShowPrompt);
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FlagHolderText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FlagTimerText;
};
