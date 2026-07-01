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

	// MGScore 갱신 테스트
	void CopyProperties(APlayerState* PlayerState) override;

	// MGScore 갱신 테스트
	// 서버가 특정 클라이언트에게만 실행하라고 지시하는 함수 (Reliable로 확실히 전달)
	UFUNCTION(Client, Reliable)
	void Client_LogScoreBeforeTravel(int32 AddedScore, int32 InTotalScore);

public:
	// Score는 APlayerState 내장 변수(float) 사용
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 Rank = 0;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 MGScore;	// 현재 미니게임 점수

	void SetMGScore(int32 InMGScore) { MGScore = InMGScore; }
	int32 GetMGScore() { return MGScore; }

	UPROPERTY(Replicated)
	int32 TotalScore;	// 게임 최종점수

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMGPlayerColor PlayerColor = EMGPlayerColor::Red;

	FLinearColor GetPlayerLinearColor() const;
};
