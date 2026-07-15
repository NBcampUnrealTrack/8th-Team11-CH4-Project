// UW_MiniMapLayout.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Type/MGTypes.h"
#include "UW_MiniMapLayout.generated.h"

class AActor;
class APawn;
class AMGMiniMapCamera;
class UCanvasPanel;
class UImage;
class UMGMinimapIconDataAsset;

// 미니맵에 띄울 타겟 정보를 담는 구조체
USTRUCT(BlueprintType)
struct FMinimapTarget
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* TargetActor = nullptr; // 추적할 액터 (다른 플레이어, 깃발, 버튼 등)

	UPROPERTY()
	UUserWidget* TargetIconWidget = nullptr; // 화면에 띄워진 아이콘 위젯
};

UCLASS()
class MINIGAMES_API UUW_MiniMapLayout : public UUserWidget
{
	GENERATED_BODY()

#pragma region MiniMap

public:
	// 미니맵에 새로운 타겟을 등록하는 함수, Actor와 Enum TargetType 사용
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void AddMinimapTarget(AActor* InActor, EMinimapTargetType TargetType);

protected:
	// 위젯이 화면에 생성될 때 호출되는 초기화 함수
	virtual void NativeConstruct() override;

	// 위젯이 화면에서 사라질 때 호출되는 함수 (Destroy와 거의 유사, 주로 Timer 종료)
	virtual void NativeDestruct() override;

	// 현재 Target 구조체에 있는 Actor가 유효하지 않으면 TArray에서 제거하는 함수
	bool CleanInvalidTarget(const FMinimapTarget& Target);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> MinimapCanvas;

	UPROPERTY(EditAnywhere, Category = "Minimap|Setup")
	TObjectPtr<UMGMinimapIconDataAsset> IconDataAsset;

	// 미니맵 위젯 크기
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	FVector2D MinimapSize = FVector2D(256.0f, 256.0f);

	// 추적 중인 전체 타겟 배열
	UPROPERTY()
	TArray<FMinimapTarget> TrackingTargets;

private:
	// 타이머가 0.1초마다 호출할 플레이어 위치 갱신 함수
	UFUNCTION()
	void UpdateIconPosition();

	// 위치 갱신을 담당할 타이머
	FTimerHandle UpdateTimerHandle;

	// 캐싱해둘 참조 카메라
	TObjectPtr<AMGMiniMapCamera> CachedMinimapCamera;

#pragma endregion

};
