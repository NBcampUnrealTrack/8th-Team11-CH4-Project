// MGButtonHUD.cpp

#include "UI/MGButtonHUD.h"

#include "Blueprint/UserWidget.h"

void AMGButtonHUD::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(IsValid(TimeHUDWidgetClass)) == false)
	{
		return;
	}

	TimeHUDWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), TimeHUDWidgetClass);
	if (IsValid(TimeHUDWidgetInstance))
	{
		TimeHUDWidgetInstance->AddToViewport();
	}
}
