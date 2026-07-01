// MGTypes.h

#pragma once

#include "CoreMinimal.h"
#include "MGTypes.generated.h"

UENUM(BlueprintType)
enum class EMinigameType : uint8
{
	PassBomb UMETA(DisplayName = "PassTheBomb"),
	ButtonOwnership UMETA(DisplayName = "ButtonOwnership"),
	FlagGame UMETA(DisplayName = "FlagGame")
};
