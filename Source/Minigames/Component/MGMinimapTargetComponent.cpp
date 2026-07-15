// MGMinimapTargetComponent.cpp

#include "Component/MGMinimapTargetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "UI/MGHUDBase.h"
#include "UI/MiniMap/UW_MiniMapLayout.h"

UMGMinimapTargetComponent::UMGMinimapTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMGMinimapTargetComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	FTimerHandle RegisterTimer;
	GetWorld()->GetTimerManager().SetTimer(
		RegisterTimer, 
		this, 
		&UMGMinimapTargetComponent::RegisterToMinimap, 
		0.2f, 
		false
	);
}

void UMGMinimapTargetComponent::RegisterToMinimap()
{
	// 1. 나(컴포넌트)를 달고 있는 부모 액터를 가져옵니다.
	AActor* OwnerActor = GetOwner();
	if (!IsValid(OwnerActor)) return;

	// 2. 로컬 플레이어의 컨트롤러 및 HUD를 찾습니다.
	APlayerController* LocalPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(LocalPC)) return;

	AMGHUDBase* MyHUD = Cast<AMGHUDBase>(LocalPC->GetHUD());
	if (!IsValid(MyHUD) || !IsValid(MyHUD->MinimapWidget)) return;

	// 3. 기본적으로는 'Other(남/사물)' 아이콘을 세팅합니다.
	TSubclassOf<UUserWidget> SelectedIconClass = OtherPlayerCharacterIcon;

	// 4. 만약 부모 액터가 플레이어(Pawn)이고, 내 컴퓨터에서 조종 중이라면 'My' 아이콘으로 교체!
	APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	if (IsValid(OwnerPawn) && OwnerPawn->IsLocallyControlled())
	{
		SelectedIconClass = MyPlayerCharacterIcon;
	}

	// 5. 미니맵 위젯에 최종적으로 부모 액터와 선택된 아이콘을 등록합니다.
	if (IsValid(SelectedIconClass))
	{
		MyHUD->MinimapWidget->AddMinimapTarget(OwnerActor, SelectedIconClass);
	}
}

