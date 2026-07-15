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
 
    MG_LOG_ROLE(LogMGNet, Log, TEXT(""));
    if (AMGPlayerState* NewPlayerState = Cast<AMGPlayerState>(PlayerState))
    {
        NewPlayerState->TotalScore = this->TotalScore;
        NewPlayerState->PlayerColor = this->PlayerColor;
        NewPlayerState->RoundScores = this->RoundScores;

        MG_LOG_ROLE(LogMGNet, Warning, TEXT("[After Travel - CopyProperties] Copied TotalScore : %d"), NewPlayerState->TotalScore);
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

    if (PlayerColorTickDelegateHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(PlayerColorTickDelegateHandle);
    }
    PlayerColorTickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::SendColorToPlayerCharacter), 0.0f);
}

bool AMGPlayerState::SendColorToPlayerCharacter(float DeltaTime)
{
    // Pawn이 지연 스폰되는 리슨 서버 환경에서 색 설정을 계속 시도
    APawn* Pawn = GetPawn();
    if (!Pawn)
    {
        return true;  // Pawn 생성될 때까지 재시도
    }

    AMGPlayerCharacter* MGPC = Cast<AMGPlayerCharacter>(Pawn);
    if (MGPC)
    {
        MGPC->FillPlayerColor();
    }

    return false;  // Pawn 획득 후 종료
}
