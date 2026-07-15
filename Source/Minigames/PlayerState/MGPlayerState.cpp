#include "PlayerState/MGPlayerState.h"
#include "Character/MGPlayerCharacter.h"
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

void AMGPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (PlayerColorTickDelegateHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(PlayerColorTickDelegateHandle);
        PlayerColorTickDelegateHandle.Reset();
    }

    Super::EndPlay(EndPlayReason);
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

FLinearColor AMGPlayerState::GetPlayerLinearColor() const
{
    return MGPlayerColorToLinear(PlayerColor);
}

void AMGPlayerState::OnRep_PlayerColor()
{
    // TestLog
    // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White,
    //     FString::Printf(TEXT("[Player %d] Color: %d"), GetPlayerId(), (uint8)PlayerColor));

    if (PlayerColorTickDelegateHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(PlayerColorTickDelegateHandle);
    }
    PlayerColorTickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::SendColorToPlayerCharacter), 0.0f);
}

bool AMGPlayerState::SendColorToPlayerCharacter(float DeltaTime)
{
    if (IsValid(GetPawn()))
    {
        AMGPlayerCharacter* MGPC = Cast<AMGPlayerCharacter>(GetPawn());

        if (IsValid(MGPC))
        {
            MGPC->FillPlayerColor();
            return false;
        }
    }
    return true;
}
