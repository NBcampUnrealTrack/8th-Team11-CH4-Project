// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MGLobbyGameStateBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnLobbyRosterChanged); // 레디/색/입퇴장 -> 목록 갱신
DECLARE_MULTICAST_DELEGATE(FOnLobbyHeaderChanged); // 인원수/카운트다운 -> 헤더 텍스트만

UCLASS()
class MINIGAMES_API AMGLobbyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetCurrentPlayerCount(int32 NewCount);
	void SetRemainCountdownTime(int32 NewTime);

	FORCEINLINE int32 GetCurrentPlayerCount() const { return CurrentPlayerCount; }
	FORCEINLINE int32 GetRemainCountdownTime() const { return RemainCountdownTime; }
	
	FOnLobbyRosterChanged OnLobbyRosterChanged;
	FOnLobbyHeaderChanged OnLobbyHeaderChanged;
	
protected:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	
	UPROPERTY(ReplicatedUsing = OnRep_HeaderChanged, VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentPlayerCount = 0;
	
	// 남은 카운트다운 시간 (10→0). 0이면 카운트다운 미진행
	UPROPERTY(ReplicatedUsing = OnRep_HeaderChanged, VisibleAnywhere, BlueprintReadOnly)
	int32 RemainCountdownTime = 0;
	
private:
	UFUNCTION()
	void OnRep_HeaderChanged();
};
