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

inline FLinearColor MGPlayerColorToLinear(EMGPlayerColor Color)
{
    switch (Color)
    {
    case EMGPlayerColor::Red:
        {
            return FLinearColor::Red;
        }

    case EMGPlayerColor::Orange:
        {
            return FLinearColor(1.f, 0.5f, 0.f);
        }

    case EMGPlayerColor::Yellow:
        {
            return FLinearColor::Yellow;
        }

    case EMGPlayerColor::Green:
        {
            return FLinearColor::Green;
        }

    case EMGPlayerColor::Blue:
        {
            return FLinearColor::Blue;
        }

    case EMGPlayerColor::Indigo:
        {
            return FLinearColor(0.29f, 0.f, 0.51f);
        }

    case EMGPlayerColor::Purple:
        {
            return FLinearColor(0.5f, 0.f, 1.f);
        }

    case EMGPlayerColor::Pink:
        {
            return FLinearColor(1.f, 0.3f, 0.8f);
        }

    case EMGPlayerColor::White:
        {
            return FLinearColor::White;
        }

    case EMGPlayerColor::Gray:
        {
            return FLinearColor::Gray;
        }

    default:
        {
            return FLinearColor::Black;
        }
    }
}
