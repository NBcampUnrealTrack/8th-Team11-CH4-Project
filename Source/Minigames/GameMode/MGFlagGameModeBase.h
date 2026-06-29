// MGFlagGameModeBase.h

#pragma once

#include "CoreMinimal.h"
#include "MGGameModeBase.h"
#include "MGFlagGameModeBase.generated.h"

UCLASS()
class MINIGAMES_API AMGFlagGameModeBase : public AMGGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	virtual void StartMiniGame() override;
	
protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	UFUNCTION()
	void OnGameTimerElapsed();
	
	// TODO: virtual 로 수정하기
	void EndGame();
	
	void DeterMineWinner();
	
	FTimerHandle GameTimerHandle;
	
	int32 RemainGameTime = 120;
};
