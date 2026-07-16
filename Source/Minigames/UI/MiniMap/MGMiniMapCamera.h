// MGMiniMapCamera.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGMiniMapCamera.generated.h"

class USceneCaptureComponent2D;
class UTextureRenderTarget2D;

UCLASS()
class MINIGAMES_API AMGMiniMapCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	AMGMiniMapCamera();

protected:
	virtual void BeginPlay() override;

public:
	// 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComp;

	// 캡처한 이미지가 저장될 RT_Minimap 에셋
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minimap")
	TObjectPtr<UTextureRenderTarget2D> MinimapRenderTarget;

	// 캡처 주기
	UPROPERTY(EditAnywhere, Category = "Minimap")
	float CaptureInterval;

	// 직교(Orthographic) 투영 시 보여질 맵의 가로 너비
	UPROPERTY(EditAnywhere, Category = "Minimap")
	float OrthographicSize;

private:
	// 캡처를 반복할 타이머 핸들러
	FTimerHandle CaptureTimerHandler;

	// 실제로 미니맵을 캡처하는 함수
	UFUNCTION()
	void CaptureMinimap();
};
