// MGFinalResultGameModeBase.h


#pragma once

#include "CoreMinimal.h"
#include "GameMode/MGGameModeBase.h"
#include "MGFinalResultGameModeBase.generated.h"

class AMGPlayerState;

UCLASS()
class MINIGAMES_API AMGFinalResultGameModeBase : public AMGGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	
	virtual void StartMinigame() override;
	
	virtual void Logout(AController* Exiting) override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void OnPlayerReadyToReturn(AMGPlayerController* PC);
	
private:
	TArray<AMGPlayerState*> GetPlayersSortedByScore() const;

	void PlacePlayersByRank(const TArray<AMGPlayerState*>& SortedPlayers);
	
	void CheckAllReadyToReturn();
	
	void ReturnToLobby();
	
	FTimerHandle AutoReturnTimerHandle;
	
	// 태그로 마커 액터 하나 찾기 (시상대용)
	AActor* GetSingleActorByTag(FName Tag) const;
};
