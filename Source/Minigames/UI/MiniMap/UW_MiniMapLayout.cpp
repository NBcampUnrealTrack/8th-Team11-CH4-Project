// UW_MiniMapLayout.cpp

#include "UI/MiniMap/UW_MiniMapLayout.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MiniMap/MGMiniMapCamera.h"
#include "Data/MGMinimapIconDataAsset.h"

void UUW_MiniMapLayout::NativeConstruct()
{
	Super::NativeConstruct();

	// 맵에 배치된 미니맵 카메라 찾기
	CachedMinimapCamera = Cast<AMGMiniMapCamera>(UGameplayStatics::GetActorOfClass(GetWorld(), AMGMiniMapCamera::StaticClass()));

	// 0.1초마다 UpdateIconPosition 함수를 실행하는 타이머 가동
	if (IsValid(CachedMinimapCamera))
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

void UUW_MiniMapLayout::AddMinimapTarget(AActor* InActor, EMinimapTargetType TargetType)
{
	if (!IsValid(InActor))
	{
		return;
	}

	if (!IsValid(IconDataAsset))
	{
		return;
	}

	if (!IsValid(MinimapCanvas))
	{
		return;
	}

	for (const FMinimapTarget& Target : TrackingTargets)
	{
		if (Target.TargetActor == InActor)
		{
			return; // 중복 등록의 경우 조기 return
		}
	}

	// 이미 아이콘이 Map에 등록되어 있으면 return
	if (!IconDataAsset->IconMap.Contains(TargetType))
	{
		return;
	}

	TSubclassOf<UUserWidget> MinimapIcon = IconDataAsset->IconMap[TargetType];
	if (!IsValid(MinimapIcon))
	{
		return;
	}	// Target Type에 맞는 Icon이 IconDataAsset에 없으면 return

	APlayerController* OwningPC = GetOwningPlayer();
	if (!IsValid(OwningPC))
	{
		return;
	}

	UUserWidget* NewIcon = nullptr;
	if (IsValid(OwningPC))
	{
		NewIcon = CreateWidget<UUserWidget>(OwningPC, MinimapIcon);
	}

	if (!IsValid(NewIcon))
	{
		return;
	}

	UCanvasPanelSlot* NewSlot = MinimapCanvas->AddChildToCanvas(NewIcon);
	if (NewSlot)
	{
		NewSlot->SetAnchors(FAnchors(0.5f));
		NewSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		NewSlot->SetAutoSize(true);
	}

	// 추적 배열에 저장
	FMinimapTarget NewTarget;
	NewTarget.TargetActor = InActor;
	NewTarget.TargetIconWidget = NewIcon;

	TrackingTargets.Add(NewTarget);
}

bool UUW_MiniMapLayout::CleanInvalidTarget(const FMinimapTarget& Target)
{
	if (!IsValid(Target.TargetActor))
	{
		if (IsValid(Target.TargetIconWidget))
		{
			Target.TargetIconWidget->RemoveFromParent();
		}
		return true;
	}
	return false;
}

void UUW_MiniMapLayout::UpdateIconPosition()
{
	TrackingTargets.RemoveAll([this](const FMinimapTarget& Target) {
		return CleanInvalidTarget(Target);
		});
	// TArray TrackingTargets에서 유효하지 않은 Actor의 Icon을 람다를 통해 제거

	if (!IsValid(CachedMinimapCamera))
	{
		return;
	}

	FVector CameraLoc = CachedMinimapCamera->GetActorLocation();
	float OrthoWidth = CachedMinimapCamera->OrthographicSize;

	// 추적 배열에 등록된 모든 타겟(Actor) 순회
	for (const FMinimapTarget& Target : TrackingTargets)
	{
		// TrackedTargets 배열에서 추적하고 있는 Actor를 꺼내왔을때 유효하지 않거나(Destroy 등)  
		// 추적할 Actor의 Icon이 없으면 다음 Target으로
		if (!IsValid(Target.TargetActor) || !IsValid(Target.TargetIconWidget))
		{
			continue;
		}

		FVector ActorLoc = Target.TargetActor->GetActorLocation();

		// 카메라 위치를 기준으로 타겟의 상대위치 구하기
		float DistanceX = ActorLoc.X - CameraLoc.X;
		float DistanceY = ActorLoc.Y - CameraLoc.Y;

		// Ratio를 [-1.0 ~ 1.0] 으로 맵핑
		float RatioX = DistanceY / (OrthoWidth * 0.5f);
		float RatioY = -DistanceX / (OrthoWidth * 0.5f);

		// 미니맵 전체 크기[-256 ~ 256] 의 절반을 곱해서 [-128 ~ 128] 맵핑
		float UI_X = RatioX * (MinimapSize.X * 0.5f);
		float UI_Y = RatioY * (MinimapSize.Y * 0.5f);

		// TargetIconWidget의 위치를 옮기기
		if (UCanvasPanelSlot* IconSlot = Cast<UCanvasPanelSlot>(Target.TargetIconWidget->Slot))
		{
			IconSlot->SetPosition(FVector2D(UI_X, UI_Y));
		}
	}
}
