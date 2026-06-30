#include "PlayerState/MGPlayerState.h"
#include "Net/UnrealNetwork.h"

AMGPlayerState::AMGPlayerState()
	: TotalScore(0)
    , MGScore(0)
    , Rank(0)
{
	bReplicates = true;
}

void AMGPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TotalScore);
	DOREPLIFETIME(ThisClass, MGScore);
	DOREPLIFETIME(ThisClass, Rank);

}

FLinearColor AMGPlayerState::GetPlayerLinearColor() const
{
    switch (PlayerColor)
    {
    case EMGPlayerColor::Red:
        return FLinearColor::Red;

    case EMGPlayerColor::Orange:
        return FLinearColor(1.f, 0.5f, 0.f);

    case EMGPlayerColor::Yellow:
        return FLinearColor::Yellow;

    case EMGPlayerColor::Green:
        return FLinearColor::Green;

    case EMGPlayerColor::Blue:
        return FLinearColor::Blue;

    case EMGPlayerColor::Indigo:
        return FLinearColor(0.29f, 0.f, 0.51f);

    case EMGPlayerColor::Purple:
        return FLinearColor(0.5f, 0.f, 1.f);

    case EMGPlayerColor::Pink:
        return FLinearColor(1.f, 0.3f, 0.8f);

    case EMGPlayerColor::White:
        return FLinearColor::White;

    case EMGPlayerColor::Gray:
        return FLinearColor::Gray;
    }

    return FLinearColor::Black;
}
