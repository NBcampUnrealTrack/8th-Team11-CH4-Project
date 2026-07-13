// MGMiniMapCamera.cpp

#include "UI/MiniMap/MGMiniMapCamera.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

AMGMiniMapCamera::AMGMiniMapCamera()
	: CaptureInterval(0.1f)
	, OrthographicSize(8000.f)
{
	PrimaryActorTick.bCanEverTick = false;	// Tick 갱신 대신 적당한 Interval 사용

	SceneCaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComp"));
	RootComponent = SceneCaptureComp;

	// 카메라 세팅 (직교 투영 = 거리에 따른 감쇠가 없도록)
	SceneCaptureComp->ProjectionType = ECameraProjectionMode::Orthographic;
	SceneCaptureComp->OrthoWidth = OrthographicSize;

	// 하늘에서 바닥을 수직으로 내려다보도록 카메라 회전
	SceneCaptureComp->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	SceneCaptureComp->bCaptureEveryFrame = false;	// 매 프레임 캡처 기능 끄기
	SceneCaptureComp->bCaptureOnMovement = false;	// 움직일 때 캡처 기능 끄기

	SceneCaptureComp->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR; // LDR 포맷으로 사용해야함
}

void AMGMiniMapCamera::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(MinimapRenderTarget))
	{
		SceneCaptureComp->TextureTarget = MinimapRenderTarget;
	}

	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	} // UI와 렌더링은 Client에게만 필요하므로 서버는 조기 return

	// 지정된 간격(0.1초)마다 수동으로 사진을 찍는 타이머 가동
	GetWorldTimerManager().SetTimer(
		CaptureTimerHandler,
		this,
		&AMGMiniMapCamera::CaptureMinimap,
		CaptureInterval,
		true // 무한 반복
	);
}

void AMGMiniMapCamera::CaptureMinimap()
{
	if (IsValid(SceneCaptureComp))
	{
		SceneCaptureComp->CaptureScene();
	}
}
