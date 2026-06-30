// MGGameInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameState/MGGameStateBase.h"
#include "MGGameInstance.generated.h"

UCLASS()
class MINIGAMES_API UMGGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	UMGGameInstance();

	//virtual void Init() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Flow")
	ERoundState CurrentRoundState;
};
