// UW_MiniMapLayout.cpp

#include "UI/MiniMap/UW_MiniMapLayout.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MiniMap/MGMiniMapCamera.h"

void UUW_MiniMapLayout::NativeConstruct()
{
	Super::NativeConstruct();

	// PlayerController가 소유중인 Pawn = 각자의 Pawn
	CachedPlayerPawn = GetOwningPlayerPawn();

	// 맵에 배치된 미니맵 카메라 찾기
	CachedMinimapCamera = Cast<AMGMiniMapCamera>(UGameplayStatics::GetActorOfClass(GetWorld(), AMGMiniMapCamera::StaticClass()));

	// 0.1초마다 UpdateIconPosition 함수를 실행하는 타이머 가동
	if (IsValid(CachedPlayerPawn) && IsValid(CachedMinimapCamera))
	{
		GetWorld()->GetTimerManager().SetTimer(
			UpdateTimerHandle,
			this,
			&UUW_MiniMapLayout::UpdateIconPosition,
			0.1f,
			true
		);
	}
}

void UUW_MiniMapLayout::NativeDestruct()
{
	// 위젯이 파괴될 때 안전하게 모든 Timer Clear
	GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);

	Super::NativeDestruct();
}

void UUW_MiniMapLayout::UpdateIconPosition()
{
	if (!IsValid(CachedPlayerPawn) || !IsValid(CachedMinimapCamera) || !IsValid(PlayerIcon))
	{
		return;
	}

	FVector PlayerLoc = CachedPlayerPawn->GetActorLocation();
	FVector CameraLoc = CachedMinimapCamera->GetActorLocation();
	float OrthoWidth = CachedMinimapCamera->OrthographicSize;

	// 카메라 위치를 기준으로 플레이어의 상대위치 구하기
	float DistanceX = PlayerLoc.X - CameraLoc.X;
	float DistanceY = PlayerLoc.Y - CameraLoc.Y;

	// Ratio를 [-1.0 ~ 1.0] 으로 맵핑
	float RatioX = DistanceY / (OrthoWidth * 0.5f);
	float RatioY = -DistanceX / (OrthoWidth * 0.5f);

	// 미니맵 전체 크기(256 * 256)의 절반을 곱해서 [-128 ~ 128] 맵핑
	float UI_X = RatioX * (MinimapSize.X * 0.5f);
	float UI_Y = RatioY * (MinimapSize.Y * 0.5f);

	if (UCanvasPanelSlot* IconSlot = Cast<UCanvasPanelSlot>(PlayerIcon->Slot))
	{
		IconSlot->SetPosition(FVector2D(UI_X, UI_Y));
	}
}
