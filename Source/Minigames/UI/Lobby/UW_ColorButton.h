// UW_ColorButton.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Type/MGPlayerColor.h"
#include "UW_ColorButton.generated.h"

class UButton;
class UImage;

// 클릭된 색을 상위(팔레트)로 전달
DECLARE_MULTICAST_DELEGATE_OneParam(FOnColorButtonClicked, EMGPlayerColor);

UCLASS()
class MINIGAMES_API UUW_ColorButton : public UUserWidget
{
	GENERATED_BODY()

public:
	// 이 버튼이 담당할 색 지정 (스와치 틴트까지 반영)
	void Setup(EMGPlayerColor InColor);

	FOnColorButtonClicked OnColorButtonClicked;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleClicked();

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> ColorButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> ColorSwatch;

	EMGPlayerColor MyColor = EMGPlayerColor::None;
};