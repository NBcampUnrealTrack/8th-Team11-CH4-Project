// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_LobbyPlayerRow.generated.h"

class UImage;
class UTextBlock;
class AMGLobbyPlayerState;

UCLASS()
class MINIGAMES_API UUW_LobbyPlayerRow : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(AMGLobbyPlayerState* PS);

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> ColorSwatch;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameText;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> ReadyText;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UImage> ReadyCheck;
};
