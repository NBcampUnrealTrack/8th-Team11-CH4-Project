// UW_MiniMapLayout.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_MiniMapLayout.generated.h"

class APawn;
class AMGMiniMapCamera;
class UCanvasPanel;
class UImage;

UCLASS()
class MINIGAMES_API UUW_MiniMapLayout : public UUserWidget
{
	GENERATED_BODY()

#pragma region MiniMap

protected:
	// 위젯이 화면에 생성될 때 호출되는 초기화 함수
	virtual void NativeConstruct() override;

	// 위젯이 화면에서 사라질 때 호출되는 함수 (Destroy와 거의 유사, 주로 Timer 종료)
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> MinimapCanvas;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerIcon;

	// 미니맵 위젯 크기
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	FVector2D MinimapSize = FVector2D(256.0f, 256.0f);

private:
	// 타이머가 0.1초마다 호출할 플레이어 위치 갱신 함수
	UFUNCTION()
	void UpdateIconPosition();

	// 위치 갱신을 담당할 타이머
	FTimerHandle UpdateTimerHandle;

	// 캐싱해둘 참조 변수들 (매번 찾으면 무거우니까)
	TObjectPtr<APawn> CachedPlayerPawn;
	TObjectPtr<AMGMiniMapCamera> CachedMinimapCamera;

#pragma endregion

};
