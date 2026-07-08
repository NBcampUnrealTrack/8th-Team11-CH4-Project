#pragma once

#include "CoreMinimal.h"
#include "PlayerState/MGPlayerState.h"
#include "MGButtonPlayerState.generated.h"

UCLASS()
class MINIGAMES_API AMGButtonPlayerState : public AMGPlayerState
{
    GENERATED_BODY()

public:
    AMGButtonPlayerState();

    UPROPERTY(BlueprintReadOnly, Replicated)
    int32 FinalRank;

    UPROPERTY(BlueprintReadOnly, Replicated)
    int32 FinalScore;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};