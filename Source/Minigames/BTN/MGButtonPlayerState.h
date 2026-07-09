#pragma once

#include "CoreMinimal.h"
#include "PlayerState/MGPlayerState.h"
#include "Net/UnrealNetwork.h" // Ãß°¡
#include "MGButtonPlayerState.generated.h"

UCLASS()
class MINIGAMES_API AMGButtonPlayerState : public AMGPlayerState
{
    GENERATED_BODY()

public:
    AMGButtonPlayerState();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void OnRep_ResultData();

    UPROPERTY(ReplicatedUsing = OnRep_ResultData, BlueprintReadOnly, Category = "Game")
    int32 FinalScore;

    UPROPERTY(ReplicatedUsing = OnRep_ResultData, BlueprintReadOnly, Category = "Game")
    int32 FinalRank;

    UFUNCTION(BlueprintCallable, Category = "Game")
    int32 GetButtonScore() const { return GetScore(); }
};