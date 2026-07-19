// MGPassBombGameState.cpp

#include "GameState/MGPassBombGameState.h"
#include "Net/UnrealNetwork.h"
#include "UI/MGPassBombHUD.h"
#include "UI/UW_PassBombLayout.h"

void AMGPassBombGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMGPassBombGameState, TotalPlayerCount);
	DOREPLIFETIME(AMGPassBombGameState, AlivePlayerCount);
}

void AMGPassBombGameState::OnRep_PlayerCount()
{
	APlayerController* LocalPC = GetWorld()->GetFirstPlayerController();
	if (IsValid(LocalPC))
	{
		AMGPassBombHUD* MyHUD = Cast<AMGPassBombHUD>(LocalPC->GetHUD());

		if (IsValid(MyHUD) && IsValid(MyHUD->BombWidgetInstance))
		{
			UUW_PassBombLayout* PassBombUI = Cast<UUW_PassBombLayout>(MyHUD->BombWidgetInstance);

			if (IsValid(PassBombUI))
			{
				PassBombUI->UpdateAliveCountText(AlivePlayerCount, TotalPlayerCount);
			}
		}
	}
}
