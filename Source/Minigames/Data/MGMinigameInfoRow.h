// MGMinigameInfoRow.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"     // FTableRowBase
#include "Type/MGTypes.h"         // EMinigameType (기존 것 재사용)
#include "MGMinigameInfoRow.generated.h"

USTRUCT(BlueprintType)
struct FMGMinigameInfoRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame")
    EMinigameType MinigameType = EMinigameType::FlagGame;

    // "게임 3 · 깃발 뺏기"
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame|Text")
    FText Title;

    // "깃발을 빼앗아 다른사람들한테서 도망쳐!"
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame|Text")
    FText Subtitle;

    // 일러스트 아래 캡션 "달려가서 깃발 잡기"
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame|Text")
    FText IllustrationCaption;

    // 규칙 줄들 (1,2,3...) — 개수 가변 대응
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame|Text", meta = (MultiLine = true))
    TArray<FText> RuleLines;

    // "목표 — ..."
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame|Text")
    FText GoalText;

    // 일러스트/아이콘
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame|Visual")
    TSoftObjectPtr<UTexture2D> Illustration;

    // 목적지 맵
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minigame|Level")
    TSoftObjectPtr<UWorld> Level;
};
