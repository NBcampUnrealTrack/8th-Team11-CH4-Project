// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_LobbyLayout.generated.h"

class UWrapBox;
class AMGLobbyGameStateBase;
class UButton;
class UTextBlock;
class UUW_LobbyPlayerRow;
class UUW_ColorButton;
enum class EMGPlayerColor : uint8;

UCLASS()
class MINIGAMES_API UUW_LobbyLayout : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void OnReadyClicked();

private:
	void RefreshRoster();
	void RefreshHeader();

	void BuildColorPalette();
	void OnColorPicked(EMGPlayerColor NewColor);
	
	AMGLobbyGameStateBase* GetLobbyGameState() const;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> ReadyButton;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> CountdownText;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerCountText;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> ReadyCountText;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UWrapBox> PlayerListBox;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UWrapBox> ColorPaletteBox;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUW_ColorButton> ColorButtonClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUW_LobbyPlayerRow> PlayerRowClass;

	// 생성해둔 줄 위젯 재사용용
	UPROPERTY()
	TArray<TObjectPtr<UUW_LobbyPlayerRow>> RowWidgets;

	// 구독 해제/유효성 위해 약참조로 보관
	TWeakObjectPtr<AMGLobbyGameStateBase> CachedGameState;
};
