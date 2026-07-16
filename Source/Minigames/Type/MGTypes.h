// MGTypes.h

#pragma once

#include "CoreMinimal.h"
#include "MGTypes.generated.h"

UENUM(BlueprintType)
enum class EMatchState : uint8
{
	None,
	Entering,			 // 심리스 트래블 후 클라이언트들이 맵에 들어오길 기다리는 상태
	Waiting,			 // 모든 클라이언트가 Waiting 상태에 진입 시 Playing으로 전환하는 Timer 시작
	PlayingCutScene,
	Playing,
	RoundResult,		 // 해당 미니게임 결과 UI 표시 (게임 조작 불가, FInputModeUIOnly)
	Ending,				 // ServerTravel을 호출하기 직전 마무리 단계
	ALL_COUNT UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ERoundState : uint8
{
	None,
	Lobby,
	Round1,
	Round2,
	Round3,
	FinalResult,			// FinalResult 상태에서 Lobby로 돌아가는 UI 표시
	ALL_COUNT UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EMinigameType : uint8
{
	NONE,
	PassBomb UMETA(DisplayName = "PassTheBomb"),
	ButtonOwnership UMETA(DisplayName = "ButtonOwnership"),
	FlagGame UMETA(DisplayName = "FlagGame")
};

UENUM(BlueprintType)
enum class EMinimapTargetType : uint8
{
	None,
	My	        UMETA(DisplayName = "자신"),
	Ally        UMETA(DisplayName = "아군"),
	Enemy       UMETA(DisplayName = "적"),
	Objective   UMETA(DisplayName = "오브젝트"),
};
