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
	// 안전 검사
	if (!IsValid(CachedPlayerPawn) || !IsValid(CachedMinimapCamera) || !IsValid(PlayerIcon))
	{
		return;
	}

	// 월드 좌표 가져오기
	FVector PlayerLoc = CachedPlayerPawn->GetActorLocation();
	FVector CameraLoc = CachedMinimapCamera->GetActorLocation();
	float OrthoWidth = CachedMinimapCamera->OrthographicSize;

	// 3D 월드 좌표 -> 2D UI 좌표 변환 공식 (언리얼 좌표계 기준)
	// X축: 플레이어의 Y(좌우) - 카메라의 Y(좌우)
	// Y축: 플레이어의 X(앞뒤) - 카메라의 X(앞뒤)에 마이너스 적용 (UI는 아래로 갈수록 +이므로)
	float UI_X = ((PlayerLoc.Y - CameraLoc.Y) / OrthoWidth) * MinimapSize.X;
	float UI_Y = (-(PlayerLoc.X - CameraLoc.X) / OrthoWidth) * MinimapSize.Y;

	// 캔버스 슬롯으로 형변환하여 위치 적용
	if (UCanvasPanelSlot* IconSlot = Cast<UCanvasPanelSlot>(PlayerIcon->Slot))
	{
		IconSlot->SetPosition(FVector2D(UI_X, UI_Y));
	}
}
