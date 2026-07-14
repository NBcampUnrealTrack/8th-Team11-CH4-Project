// MGGameStateBase.h

#pragma once

#include "GameFramework/GameStateBase.h"
#include "Type/MGTypes.h"
#include "MGGameStateBase.generated.h"

UCLASS()
class MINIGAMES_API AMGGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_MatchState();
	
public:
	UPROPERTY(ReplicatedUsing = OnRep_MatchState, VisibleAnywhere, BlueprintReadOnly, Category = "GameFlow")
	EMatchState MatchState = EMatchState::Waiting;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameFlow")
	ERoundState RoundState = ERoundState::None;

	// 생존한 캐릭터 (컨트롤러는 클라로 넘길 수 없으므로 캐릭터로 넘김)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameFlow")
	TArray<TObjectPtr<class ACharacter>> AliveCharacters;
};
