// UW_PlayerNameText.cpp


#include "UI/UW_PlayerNameText.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UUW_PlayerNameText::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APawn* OwningPawn = Cast<APawn>(OwningActor);
	if (IsValid(OwningPawn) == false)
	{
		return;
	}

	APlayerState* PS = OwningPawn->GetPlayerState();
	if (IsValid(PS) == false)
	{
		return;
	}

	const FString CurrentName = PS->GetPlayerName();
	if (CurrentName != LastName)
	{
		LastName = CurrentName;
		PlayerNameText->SetText(FText::FromString(CurrentName));
	}
}
