// MGNameWidgetComponent.cpp


#include "Component/MGNameWidgetComponent.h"

#include "UI/UW_PlayerNameText.h"

void UMGNameWidgetComponent::InitWidget()
{
	Super::InitWidget();

	if (UUW_PlayerNameText* NameText = Cast<UUW_PlayerNameText>(GetWidget()))
	{
		NameText->SetOwningActor(GetOwner());
	}
}
