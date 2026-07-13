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
            return FLinearColor(1.000f, 0.420f, 0.420f);
        }

    case EMGPlayerColor::Orange:
        {
            return FLinearColor(1.000f, 0.624f, 0.251f);
        }

    case EMGPlayerColor::Yellow:
        {
            return FLinearColor(1.000f, 0.780f, 0.239f);
        }

    case EMGPlayerColor::Green:
        {
            return FLinearColor(0.424f, 0.800f, 0.467f);
        }

    case EMGPlayerColor::Blue:
        {
            return FLinearColor(0.310f, 0.804f, 0.773f);
        }

    case EMGPlayerColor::Indigo:
        {
            return FLinearColor(0.302f, 0.608f, 0.961f);
        }

    case EMGPlayerColor::Purple:
        {
            return FLinearColor(0.631f, 0.416f, 0.961f);
        }

    case EMGPlayerColor::Pink:
        {
            return FLinearColor(0.996f, 0.420f, 0.835f);
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
