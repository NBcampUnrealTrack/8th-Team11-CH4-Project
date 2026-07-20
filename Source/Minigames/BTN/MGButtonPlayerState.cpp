#include "MGButtonPlayerState.h"

void AMGButtonPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	OnButtonScoreChanged.Broadcast();
}