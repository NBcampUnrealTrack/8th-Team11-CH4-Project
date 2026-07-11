#pragma once

#include "CoreMinimal.h"
#include "Type/MGPlayerColor.h"
#include "MGChatType.generated.h"

USTRUCT(BlueprintType)
struct FMGChatType
{
	GENERATED_BODY()

	UPROPERTY()
	FString SenderName;

	UPROPERTY()
	EMGPlayerColor SenderColor = EMGPlayerColor::None;

	UPROPERTY()
	FString Message;
};