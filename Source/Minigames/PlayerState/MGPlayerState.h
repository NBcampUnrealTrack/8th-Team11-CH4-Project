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

	void CopyProperties(APlayerState* PlayerState) override;
	
	void SetMGScore(int32 InMGScore) { MGScore = InMGScore; }
	
	int32 GetMGScore() { return MGScore; }
	
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

	UPROPERTY(ReplicatedUsing = OnRep_PlayerColor, Replicated, EditAnywhere, BlueprintReadWrite)
	EMGPlayerColor PlayerColor = EMGPlayerColor::None;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	bool bReadyToReturn = false;
	
private:
	UFUNCTION()
	void OnRep_PlayerColor();
};
