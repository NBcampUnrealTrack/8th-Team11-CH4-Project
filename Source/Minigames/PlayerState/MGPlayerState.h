// MGPlayerState.h

#pragma once

#include "GameFramework/PlayerState.h"
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

public:
	// Score는 APlayerState 내장 변수(float) 사용
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 Rank = 0;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 MGScore;	// 현재 미니게임 점수

	UPROPERTY(Replicated)
	int32 TotalScore;	// 게임 최종점수
};
