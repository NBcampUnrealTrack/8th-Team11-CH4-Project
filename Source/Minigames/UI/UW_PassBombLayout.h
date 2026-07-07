// UW_PassBombLayout.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_PassBombLayout.generated.h"

class UTextBlock;

UCLASS()
class MINIGAMES_API UUW_PassBombLayout : public UUserWidget
{
	GENERATED_BODY()

#pragma region BombHolder

public:
	void UpdateBombHolderText(const FString& NewHolderName);

	void UpdateBombTimerText(int32 RemainTime);

protected:
	// meta = (BindWidget)을 쓰면 블루프린트에 있는 똑같은 이름의 UI 요소를 C++와 자동 연결
	// && 구현할 것을 요구(강제)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* BombHolderText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BombTimerText;

#pragma endregion
	
};
