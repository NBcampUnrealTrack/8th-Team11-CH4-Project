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
    
    DOREPLIFETIME(ThisClass, bReadyToReturn);
    DOREPLIFETIME(ThisClass, PlayerColor);
    DOREPLIFETIME(ThisClass, RoundScores);
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
        NewPlayerState->RoundScores = this->RoundScores;

        UE_LOG(LogTemp, Warning, TEXT("[After Travel - CopyProperties] Copied TotalScore : %d"), NewPlayerState->TotalScore);
    }
}

void AMGPlayerState::SetPlayerColor(EMGPlayerColor NewColor)
{
    PlayerColor = NewColor;

    OnRep_PlayerColor();
}

FLinearColor AMGPlayerState::GetPlayerLinearColor() const
{
    return MGPlayerColorToLinear(PlayerColor);
}

void AMGPlayerState::OnRep_PlayerColor()
{
    // TestLog
    // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
    //     FString::Printf(TEXT("[Player %d] Color: %d"), GetPlayerId(), (uint8)PlayerColor));
}
