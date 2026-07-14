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
            return FLinearColor(0.957f, 0.208f, 0.271f); 
        }

    case EMGPlayerColor::Orange:
        {
            return FLinearColor(1.000f, 0.537f, 0.004f);
        }

    case EMGPlayerColor::Yellow:
        {
            return FLinearColor(0.980f, 0.843f, 0.090f); 
        }

    case EMGPlayerColor::Green:
        {
            return FLinearColor(0.000f, 0.729f, 0.443f);
        }

    case EMGPlayerColor::Blue:
        {
            return FLinearColor(0.000f, 0.761f, 0.871f); 
        }

    case EMGPlayerColor::Indigo:
        {
            return FLinearColor(0.000f, 0.255f, 0.553f); 
        }

    case EMGPlayerColor::Purple:
        {
        return FLinearColor(0.373f, 0.157f, 0.475f); 
        }
    case EMGPlayerColor::Pink:
        {
            return FLinearColor(0.941f, 0.204f, 0.643f);
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
