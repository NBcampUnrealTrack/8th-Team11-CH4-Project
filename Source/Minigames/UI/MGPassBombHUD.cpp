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

	UUW_PassBombLayout* MyBombWidget = Cast<UUW_PassBombLayout>(BombWidgetInstance);

	if (!IsValid(MyBombWidget))
	{
		return;
	}

	AMGBombActor* FoundBomb = Cast<AMGBombActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AMGBombActor::StaticClass()));

	if (IsValid(FoundBomb))
	{
		UE_LOG(LogTemp, Warning, TEXT("Success to find Bomb"));
		FoundBomb->OnBombHolderChanged.AddUObject(this, &AMGPassBombHUD::OnBombHolderUpdated);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Fail to find Bomb"));
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
		FString PlayerName = NewHolder->GetPlayerState()->GetPlayerName();
		MyBombWidget->UpdateBombHolderText(PlayerName);
	}
}

