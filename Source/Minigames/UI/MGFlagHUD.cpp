// MGFlagHUD.cpp

#include "UI/MGFlagHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/UW_FlagLayout.h"
#include "GameState/MGFlagGameStateBase.h"
#include "PlayerState/MGFlagPlayerState.h"
#include "Character/MGPlayerCharacter.h"
#include "Component/MGFlagActorComponent.h"

void AMGFlagHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(FlagWidgetClass))
	{
		return;
	}

	FlagWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), FlagWidgetClass);
	if (IsValid(FlagWidgetInstance))
	{
		FlagWidgetInstance->AddToViewport();
	}
	
	if (AMGFlagGameStateBase* FGS = GetWorld()->GetGameState<AMGFlagGameStateBase>())
	{
		BindWithGameState(FGS);
	}
}

void AMGFlagHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(StealPromptTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void AMGFlagHUD::BindWithGameState(AMGFlagGameStateBase* FGS)
{
	if (!IsValid(FGS))
	{
		return;
	}
	
	FGS->OnRemainTimeChanged.AddUObject(this, &AMGFlagHUD::OnRemainTimeUpdated);
	FGS->OnFlagHolderChanged.AddUObject(this, &AMGFlagHUD::OnFlagHolderUpdated);

	// 구독 직후 현재값으로 즉시 1회 갱신
	OnRemainTimeUpdated(FGS->RemainGameTime);
	OnFlagHolderUpdated(FGS->GetCurrentFlagHolder());

	//0.2초마다 깃발 소유자와 거리 체크
	GetWorld()->GetTimerManager().SetTimer(
		StealPromptTimerHandle,
		this,
		&AMGFlagHUD::CheckStealPromptDistance,
		0.2f,
		true
	);
}

void AMGFlagHUD::BindPlayerFlagComponent(UMGFlagActorComponent* FlagComp)
{
	if (!IsValid(FlagComp)) return;

	FlagComp->OnFlagStateChanged.AddWeakLambda(this, [this, FlagComp](bool bHasFlag) {
		if (UUW_FlagLayout* Layout = Cast<UUW_FlagLayout>(FlagWidgetInstance))
		{
			Layout->UpdateFlagStateUI(bHasFlag, FlagComp->GetIsFlagProtected());
		}
		});

	FlagComp->OnFlagProtectionChanged.AddWeakLambda(this, [this, FlagComp](bool bIsProtected) {
		if (UUW_FlagLayout* Layout = Cast<UUW_FlagLayout>(FlagWidgetInstance))
		{
			Layout->UpdateFlagStateUI(FlagComp->GetHasFlag(), bIsProtected);
		}
		});
}

void AMGFlagHUD::OnRemainTimeUpdated(int32 RemainTime)
{
	if (UUW_FlagLayout* MyWidget = Cast<UUW_FlagLayout>(FlagWidgetInstance))
	{
		MyWidget->UpdateFlagTimerText(RemainTime);
	}

	UpdateLeaderboard();
}

void AMGFlagHUD::OnFlagHolderUpdated(AMGFlagPlayerState* NewHolder)
{
	UUW_FlagLayout* MyWidget = Cast<UUW_FlagLayout>(FlagWidgetInstance);
	if (!IsValid(MyWidget))
	{
		return;
	}

	FString HolderName = IsValid(NewHolder) ? NewHolder->GetPlayerName() : TEXT("없음");
	MyWidget->UpdateFlagHolderText(HolderName);
}

void AMGFlagHUD::UpdateLeaderboard()
{
	AMGFlagGameStateBase* FGS = Cast<AMGFlagGameStateBase>(GetWorld()->GetGameState());
	if (!IsValid(FGS)) return;

	//GameState에 등록된 PlayerArray 복사 및 필터링
	TArray<AMGFlagPlayerState*> FlagPlayerStates;
	for (APlayerState* PS : FGS->PlayerArray)
	{
		if (AMGFlagPlayerState* FlagPS = Cast<AMGFlagPlayerState>(PS))
		{
			FlagPlayerStates.Add(FlagPS);
		}
	}

	//깃발 보유 시간 기준 내림차순 정렬
	FlagPlayerStates.Sort([](const AMGFlagPlayerState& A, const AMGFlagPlayerState& B) 
		{
			if (A.HoldingTime != B.HoldingTime)
			{
				return A.HoldingTime > B.HoldingTime;
			}

			return A.GetPlayerName() < B.GetPlayerName();
		});

	if (UUW_FlagLayout* Layout = Cast<UUW_FlagLayout>(FlagWidgetInstance))
	{
		Layout->UpdateLeaderboardUI(FlagPlayerStates);
	}
}

void AMGFlagHUD::CheckStealPromptDistance()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!IsValid(PC)) return;

	AMGPlayerCharacter* MyChar = Cast<AMGPlayerCharacter>(PC->GetPawn());
	if (!IsValid(MyChar)) return;

	UMGFlagActorComponent* MyFlagComp = MyChar->GetComponentByClass<UMGFlagActorComponent>();
	if (IsValid(MyFlagComp) && MyFlagComp->GetHasFlag())
	{
		if (UUW_FlagLayout* Layout = Cast<UUW_FlagLayout>(FlagWidgetInstance))
		{
			Layout->UpdateStealPromptUI(false);
		}
		return;
	}

	//현재 GameState에 등록된 다른 모든 캐릭터들을 순회하며 거리 계산
	AMGFlagGameStateBase* FGS = Cast<AMGFlagGameStateBase>(GetWorld()->GetGameState());
	if (!IsValid(FGS)) return;

	bool bCanStealRightNow = false;

	for (APlayerState* PS : FGS->PlayerArray)
	{
		if (PS == PC->PlayerState) continue;

		APawn* TargetPawn = PS->GetPawn();
		AMGPlayerCharacter* TargetChar = Cast<AMGPlayerCharacter>(TargetPawn);
		if (!IsValid(TargetChar)) continue;

		//깃발 보유 중인 플레이어가 무적인지 체크
		UMGFlagActorComponent* TargetFlagComp = TargetChar->GetComponentByClass<UMGFlagActorComponent>();
		if (IsValid(TargetFlagComp) && TargetFlagComp->GetHasFlag() && !TargetFlagComp->GetIsFlagProtected())
		{
			float Distance = FVector::Dist(MyChar->GetActorLocation(), TargetChar->GetActorLocation());
			if (Distance <= MyFlagComp->GetStealRange())
			{
				bCanStealRightNow = true;
				break;
			}
		}
	}

	if (UUW_FlagLayout* Layout = Cast<UUW_FlagLayout>(FlagWidgetInstance))
	{
			Layout->UpdateStealPromptUI(bCanStealRightNow);
	}
}
