// MGPassBombHUD.h

#pragma once

#include "CoreMinimal.h"
#include "UI/MGHUDBase.h"
#include "MGPassBombHUD.generated.h"

class ACharacter;
class AMGBombActor;

UCLASS()
class MINIGAMES_API AMGPassBombHUD : public AMGHUDBase
{
	GENERATED_BODY()
	
public:
	AMGPassBombHUD();

	void BindWithBombActor(AMGBombActor* BombInstance);

protected:
	virtual void BeginPlay() override;

	void OnBombHolderUpdated(ACharacter* NewHolder);

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> BombWidgetClass;

private:
	// 생성된 위젯을 메모리에 들고 있을 포인터 (가비지 컬렉션 방지)
	UPROPERTY()
	class UUserWidget* BombWidgetInstance;

};
