// UW_MinigameIntro.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MGMinigameInfoRow.h"
#include "Fonts/SlateFontInfo.h"
#include "UW_MinigameIntro.generated.h"

class UVerticalBox;
class UImage;
class UTextBlock;

UCLASS()
class MINIGAMES_API UUW_MinigameIntro : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void Setup(const FMGMinigameInfoRow& Row);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loading")
	FSlateFontInfo RuleLineFont;
	
private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Title;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Subtitle;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_IllustrationCaption;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Goal;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> Image_Illustration;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UVerticalBox> Box_Rules;

};
