// MGGameStateBase.h

#pragma once

#include "GameFramework/GameStateBase.h"
#include "MGGameStateBase.generated.h"

UENUM(BlueprintType)
enum class EMatchState : uint8
{
	None, 
	Waiting,
	Playing,
	Ending,
	End
};

UENUM(BlueprintType)
enum class ERoundState : uint8
{
	Round1,
	Round2,
	Round3,
	FinalResult
};

/**
 *
 */
UCLASS()
class MINIGAMES_API AMGGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 AlivePlayerControllerCount = 0;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "GameFlow")
	EMatchState MatchState = EMatchState::Waiting;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameFlow")
	ERoundState RoundState = ERoundState::Round1;

};
