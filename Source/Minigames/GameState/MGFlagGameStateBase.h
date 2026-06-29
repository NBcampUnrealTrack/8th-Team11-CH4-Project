// MGFlagGameStateBase.h

#pragma once

#include "CoreMinimal.h"
#include "MGGameStateBase.h"
#include "GameFramework/GameStateBase.h"
#include "MGFlagGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class MINIGAMES_API AMGFlagGameStateBase : public AMGGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 RemainGameTime;
};
