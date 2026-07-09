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
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> FlagWidgetClass;
	
protected:
	virtual void BeginPlay() override;
	
	void OnRemainTimeUpdated(int32 RemainTime);
	void OnFlagHolderUpdated(AMGFlagPlayerState* NewHolder);

private:
	UPROPERTY()
	UUserWidget* FlagWidgetInstance = nullptr;
};
