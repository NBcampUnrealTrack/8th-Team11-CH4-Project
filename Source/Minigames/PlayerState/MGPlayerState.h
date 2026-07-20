// MGPlayerState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Type/MGPlayerColor.h"
#include "MGPlayerState.generated.h"

/**
 *
 */
UCLASS()
class MINIGAMES_API AMGPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMGPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void CopyProperties(APlayerState* PlayerState) override;

	void SetMGScore(int32 InMGScore) { MGScore = InMGScore; }

	void SetPlayerColor(EMGPlayerColor NewColor);

	int32 GetMGScore() { return MGScore; }

	UFUNCTION(BlueprintPure, Category = "Player")
	FLinearColor GetPlayerLinearColor() const;

public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 Rank = 0;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 MGScore = 0;	// 현재 미니게임 점수

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<int32> RoundScores;    // 라운드별 획득 점수

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 TotalScore = 0;	// 게임 최종점수

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	bool bReadyToReturn = false;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerColor, Replicated, EditAnywhere, BlueprintReadWrite)
	EMGPlayerColor PlayerColor = EMGPlayerColor::None;

private:
	UFUNCTION()
	void OnRep_PlayerColor();

	FTSTicker::FDelegateHandle PlayerColorTickDelegateHandle;

	bool SendColorToPlayerCharacter(float DeltaTime);
};