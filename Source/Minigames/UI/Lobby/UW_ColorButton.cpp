// UW_ColorButton.cpp

#include "UW_ColorButton.h"

#include "Components/Button.h"
#include "Components/Image.h"

void UUW_ColorButton::NativeConstruct()
{
	Super::NativeConstruct();

	ColorButton->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
}

void UUW_ColorButton::Setup(EMGPlayerColor InColor)
{
	MyColor = InColor;

	if (IsValid(ColorSwatch))
	{
		ColorSwatch->SetColorAndOpacity(MGPlayerColorToLinear(InColor));
	}
}

void UUW_ColorButton::HandleClicked()
{
	OnColorButtonClicked.Broadcast(MyColor);
}