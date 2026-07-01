#include "PlayerState/MGPlayerState.h"
#include "Net/UnrealNetwork.h"

#include "Minigames.h"				// 커스텀 Log

AMGPlayerState::AMGPlayerState()
	: TotalScore(0),
    MGScore(0)      // 초기화 빠짐
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

// MGScore 갱신 테스트
void AMGPlayerState::CopyProperties(APlayerState* PlayerState)
{
    Super::CopyProperties(PlayerState);

    // 새로 만들어질 객체 = PlayerState

    if (AMGPlayerState* NewPlayerState = Cast<AMGPlayerState>(PlayerState))
    {
        // 이전 PlayerState의 커스텀 변수 값을 새 PlayerState로 복사
        NewPlayerState->SetMGScore(this->GetMGScore());

        // 트래블 직후 복사 완료를 확인하는 로그 추가
        MG_LOG_NET(LogMGNet, Warning, TEXT("[After Travel - CopyProperties] Copied MGScore : %d"), NewPlayerState->GetMGScore());
    }
}

void AMGPlayerState::Client_LogScoreBeforeTravel_Implementation(int32 AddedScore, int32 InTotalScore)
{
    // 이 함수는 오직 해당 PlayerState의 주인이 되는 클라이언트에서만 실행됩니다.
    MG_LOG_NET(LogMGNet, Warning, TEXT("[Before Travel] Player : %s | Added : %d | Total MGScore : %d"),
        *GetPlayerName(), AddedScore, InTotalScore);
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
