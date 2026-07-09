// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MGChatLine.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class MINIGAMES_API UMGChatLine : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMessage(const FString& InMessage);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Message;
};
