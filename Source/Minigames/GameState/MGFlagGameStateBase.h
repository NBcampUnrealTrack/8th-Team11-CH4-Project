// MGFlagGameStateBase.h

#pragma once

#include "CoreMinimal.h"
#include "MGGameStateBase.h"
#include "GameFramework/GameStateBase.h"
#include "PlayerState/MGFlagPlayerState.h"
#include "MGFlagGameStateBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemainTimeChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnFlagHolderChanged, AMGFlagPlayerState*);

UCLASS()
class MINIGAMES_API AMGFlagGameStateBase : public AMGGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetCurrentFlagHolder(AMGFlagPlayerState* NewHolder);
	FORCEINLINE AMGFlagPlayerState* GetCurrentFlagHolder() const { return CurrentFlagHolder; }
	
	UPROPERTY(ReplicatedUsing = OnRep_RemainGameTime, VisibleAnywhere, BlueprintReadOnly)
	int32 RemainGameTime;
	
	FOnRemainTimeChanged OnRemainTimeChanged;
	FOnFlagHolderChanged OnFlagHolderChanged;
	
protected:
	UFUNCTION()
	void OnRep_RemainGameTime();
	
	UFUNCTION()
	void OnRep_CurrentFlagHolder();
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentFlagHolder, VisibleAnywhere, BlueprintReadOnly)
	AMGFlagPlayerState* CurrentFlagHolder = nullptr;
};
