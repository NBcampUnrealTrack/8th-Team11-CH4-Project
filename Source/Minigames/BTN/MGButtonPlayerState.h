#pragma once

#include "CoreMinimal.h"
#include "PlayerState/MGPlayerState.h"
#include "MGButtonPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonScoreChanged);

UCLASS()
class MINIGAMES_API AMGButtonPlayerState : public AMGPlayerState
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FOnButtonScoreChanged OnButtonScoreChanged;

protected:

	virtual void OnRep_Score() override;
};