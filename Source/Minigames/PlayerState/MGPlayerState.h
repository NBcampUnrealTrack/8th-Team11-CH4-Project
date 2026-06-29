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

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// Score는 APlayerState 내장 변수(float) 사용
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 Rank = 0;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	UPROPERTY(Replicated)
	int32 TotalScore;	// 게임 최종점수
};
