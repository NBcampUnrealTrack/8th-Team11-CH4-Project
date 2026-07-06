// MGPassBombHUD.cpp

#include "UI/MGPassBombHUD.h"

#include "Blueprint/UserWidget.h"

#include "UI/UW_PassBombLayout.h"
#include "Gimmick/MGBombActor.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h" // 닉네임을 가져오기 위해 추가

AMGPassBombHUD::AMGPassBombHUD() 
//	: BombHolderText(NAME_None)
{
}

void AMGPassBombHUD::BeginPlay()
{
	// 공통 UI ( 부모 클래스 MGHUDBase )
	Super::BeginPlay();

	if (!IsValid(BombWidgetClass))
	{
		return;
	}

	BombWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), BombWidgetClass);

	if (IsValid(BombWidgetInstance))
	{
		BombWidgetInstance->AddToViewport();
	}
}

void AMGPassBombHUD::BindWithBombActor(AMGBombActor* BombInstance)
{
	if (IsValid(BombInstance))
	{
		BombInstance->OnBombHolderChanged.AddUObject(this, &AMGPassBombHUD::OnBombHolderUpdated);

		// 델리게이트(BombHolder가 누구인지 Broadcast) 바인드 직후에 이미 BombHolder가 누구인지 아는 경우
		if (IsValid(BombInstance->BombHolder))
		{ // Broadcast를 기다리지 말고 현재 BombHolder로 UI 갱신
			OnBombHolderUpdated(BombInstance->BombHolder);
		}
	}
}


// MGBombActor에 존재하는 델리게이트에 바인딩 되어있음
void AMGPassBombHUD::OnBombHolderUpdated(ACharacter* NewHolder)
{
	UUW_PassBombLayout* MyBombWidget = Cast<UUW_PassBombLayout>(BombWidgetInstance);
	if (!IsValid(MyBombWidget))
	{
		return;
	}

	if (IsValid(NewHolder) && IsValid(NewHolder->GetPlayerState()))
	{
		// TODO : 현재 GetPlayerName() 대신
		// Lobby 또는 EOS에서 Lobby 진입 전에 설정한 유저 닉네임으로 변경
		FString PlayerName = NewHolder->GetPlayerState()->GetPlayerName();
		MyBombWidget->UpdateBombHolderText(PlayerName);
	}
}

