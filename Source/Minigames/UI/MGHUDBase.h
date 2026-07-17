// MGHUDBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MGHUDBase.generated.h"

class UUW_MiniMapLayout;

UCLASS()
class MINIGAMES_API AMGHUDBase : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void TryBindGameStateDelegate();

	UFUNCTION()
	void InitializeMinimap();

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUW_MiniMapLayout> MinimapWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UUW_MiniMapLayout> MinimapWidget;

protected:
	FTimerHandle BindTimerHandler;

};
