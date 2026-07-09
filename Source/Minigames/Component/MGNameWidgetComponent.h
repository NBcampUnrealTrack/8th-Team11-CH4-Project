// MGNameWidgetComponent.h


#pragma once

#include "Components/WidgetComponent.h"
#include "MGNameWidgetComponent.generated.h"

UCLASS()
class MINIGAMES_API UMGNameWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	virtual void InitWidget() override;
};
