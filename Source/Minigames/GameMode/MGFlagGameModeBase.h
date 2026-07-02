// MGFlagGameModeBase.h

#pragma once

#include "CoreMinimal.h"
#include "MGGameModeBase.h"
#include "MGFlagGameModeBase.generated.h"

class AMGFlagActor;

UCLASS()
class MINIGAMES_API AMGFlagGameModeBase : public AMGGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	virtual void StartMinigame() override;
	
	virtual void EndMinigame() override;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RemainGameTime = 120;
	
private:
	UFUNCTION()
	void OnGameTimerElapsed();
	
	void DetermineWinner();
	
	void SpawnFlag();
	
	FTimerHandle GameTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMGFlagActor> FlagClass;

};
