#include "PlayerState/MGPlayerState.h"
#include "Net/UnrealNetwork.h"

#include "Minigames.h"				// 커스텀 Log

AMGPlayerState::AMGPlayerState()
{
	bReplicates = true;
}

void AMGPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(ThisClass, PlayerColor);
	DOREPLIFETIME(ThisClass, TotalScore);
	DOREPLIFETIME(ThisClass, MGScore);
	DOREPLIFETIME(ThisClass, Rank);
}

void AMGPlayerState::CopyProperties(APlayerState* PlayerState)
{
    Super::CopyProperties(PlayerState);
    
    if (AMGPlayerState* NewPlayerState = Cast<AMGPlayerState>(PlayerState))
    {
        NewPlayerState->TotalScore = this->TotalScore;
        NewPlayerState->PlayerColor = this->PlayerColor;

        UE_LOG(LogTemp, Warning, TEXT("[After Travel - CopyProperties] Copied TotalScore : %d"), NewPlayerState->TotalScore);
    }
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
        
    default:
        return FLinearColor::Black;
    }
}

void AMGPlayerState::OnRep_PlayerColor()
{
    // TestLog
    // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
    //     FString::Printf(TEXT("[Player %d] Color: %d"), GetPlayerId(), (uint8)PlayerColor));
}
