// MGPassBombGameState.h

#pragma once

#include "CoreMinimal.h"
#include "GameState/MGGameStateBase.h"
#include "MGPassBombGameState.generated.h"

UCLASS()
class MINIGAMES_API AMGPassBombGameState : public AMGGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_PlayerCount();

public:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerCount)
	int32 TotalPlayerCount;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerCount)
	int32 AlivePlayerCount;
};
