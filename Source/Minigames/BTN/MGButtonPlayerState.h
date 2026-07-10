#pragma once

#include "CoreMinimal.h"
#include "PlayerState/MGPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "MGButtonPlayerState.generated.h"

UCLASS()
class MINIGAMES_API AMGButtonPlayerState : public AMGPlayerState
{
    GENERATED_BODY()

public:
    AMGButtonPlayerState();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};