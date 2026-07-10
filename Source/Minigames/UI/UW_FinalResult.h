// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_FinalResult.generated.h"

class UButton;
class UTextBlock;
class UVerticalBox;
class UUW_FinalResultRow;

UCLASS()
class MINIGAMES_API UUW_FinalResult : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnReturnClicked();
	
private:
	void RefreshBoard();

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UVerticalBox> PlayerListBox;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> ReturnButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> ReadyCountText;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUW_FinalResultRow> PlayerRowClass;

	// 생성한 줄 위젯 재사용용
	UPROPERTY()
	TArray<TObjectPtr<UUW_FinalResultRow>> RowWidgets;

	FTimerHandle RefreshTimerHandle;
};
