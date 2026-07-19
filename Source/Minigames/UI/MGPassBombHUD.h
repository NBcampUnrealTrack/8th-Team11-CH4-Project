// MGPassBombHUD.h

#pragma once

#include "CoreMinimal.h"
#include "UI/MGHUDBase.h"
#include "MGPassBombHUD.generated.h"

class ACharacter;
class AMGBombActor;
class UUserWidget;

UCLASS()
class MINIGAMES_API AMGPassBombHUD : public AMGHUDBase
{
	GENERATED_BODY()
	
public:
	AMGPassBombHUD();

	void BindWithBombActor(AMGBombActor* BombInstance);

	virtual void InitializeMinimap() override;

protected:
	virtual void BeginPlay() override;

	void OnBombHolderUpdated(ACharacter* NewHolder);

	void OnBombTimeUpdated(int32 RemainTime);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void TryBindBombGameState();

	UFUNCTION()
	void RefreshBombAliveCount();

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> BombWidgetClass;

	// 생성된 위젯을 메모리에 들고 있을 포인터 (가비지 컬렉션 방지)
	UPROPERTY()
	UUserWidget* BombWidgetInstance;

protected:
	UPROPERTY()
	ACharacter* CurrentBombHolder;	// HUD에서 따로 캐싱해둘 BombHolder

	FTimerHandle BombDelegateTimerHandler;

};
