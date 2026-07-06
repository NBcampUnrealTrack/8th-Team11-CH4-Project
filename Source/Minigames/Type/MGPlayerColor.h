#pragma once

#include "CoreMinimal.h"
#include "MGPlayerColor.generated.h"

UENUM(BlueprintType)
enum class EMGPlayerColor : uint8
{
    None,
    Red,
    Orange,
    Yellow,
    Green,
    Blue,
    Indigo,
    Purple,
    Pink,
    White,
    Gray,
    End
};