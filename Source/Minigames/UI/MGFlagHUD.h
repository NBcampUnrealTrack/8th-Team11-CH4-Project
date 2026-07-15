// MGFlagHUD.h

#pragma once

#include "CoreMinimal.h"
#include "UI/MGHUDBase.h"
#include "MGFlagHUD.generated.h"

class AMGFlagGameStateBase;
class UUserWidget;
class AMGFlagPlayerState;

UCLASS()
class MINIGAMES_API AMGFlagHUD : public AMGHUDBase
{
	GENERATED_BODY()
	
public:
	void BindWithGameState(AMGFlagGameStateBase* FGS);

	//델리게이트 바인딩 타이밍을 UI에서 분리해서 캐릭터 Input Component 셋업 타이밍에 호출하도록 이관
	void BindPlayerFlagComponent(class UMGFlagActorComponent* FlagComp);
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> FlagWidgetClass;
	
protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void OnRemainTimeUpdated(int32 RemainTime);
	void OnFlagHolderUpdated(AMGFlagPlayerState* NewHolder);

	void UpdateLeaderboard();

private:
	UPROPERTY()
	UUserWidget* FlagWidgetInstance = nullptr;

	FTimerHandle StealPromptTimerHandle;
	void CheckStealPromptDistance();
};
