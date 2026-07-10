// UW_FinalResultRow.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_FinalResultRow.generated.h"

class AMGPlayerState;
class AMGLobbyPlayerState;
class UTextBlock;
class UImage;

UCLASS()
class MINIGAMES_API UUW_FinalResultRow : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Setup(AMGPlayerState* PS, int32 Rank);

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> RankText;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> ColorSwatch;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameText;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> Round1Text;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> Round2Text;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> Round3Text;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> TotalText;
};
