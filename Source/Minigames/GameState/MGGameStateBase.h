// MGGameStateBase.h

#pragma once

#include "GameFramework/GameStateBase.h"
#include "MGGameStateBase.generated.h"

UENUM(BlueprintType)
enum class EMatchState : uint8
{
	None UMETA(DisplayName = "None"),
	Waiting UMETA(DisplayName = "Waiting"),
	Playing UMETA(DisplayName = "Playing"),
	Ending UMETA(DisplayName = "Ending"),
	ALL_COUNT UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ERoundState : uint8
{
	Round1 UMETA(DisplayName = "Round1"),
	Round2 UMETA(DisplayName = "Round2"),
	Round3 UMETA(DisplayName = "Round3"),
	FinalResult UMETA(DisplayName = "FinalResult")
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
	int32 AllPlayerControllerCount = 0;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "GameFlow")
	EMatchState MatchState = EMatchState::Waiting;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameFlow")
	ERoundState RoundState = ERoundState::Round1;

};
