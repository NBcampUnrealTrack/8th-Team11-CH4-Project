// MGButtonHUD.h

#pragma once

#include "CoreMinimal.h"
#include "UI/MGHUDBase.h"
#include "MGButtonHUD.generated.h"

class UUserWidget;

UCLASS()
class MINIGAMES_API AMGButtonHUD : public AMGHUDBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> TimeHUDWidgetClass;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UUserWidget> TimeHUDWidgetInstance = nullptr;
};
