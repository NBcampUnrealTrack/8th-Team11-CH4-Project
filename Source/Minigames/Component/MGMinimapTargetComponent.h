// MGMinimapTargetComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Type/MGTypes.h"
#include "MGMinimapTargetComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINIGAMES_API UMGMinimapTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMGMinimapTargetComponent();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void TryBindMinigameStart();

	UFUNCTION()
	void RegisterToMinimap();

protected:
	UPROPERTY(EditAnywhere, Category = "Minimap")
	EMinimapTargetType TargetType = EMinimapTargetType::None;



private:
	FTimerHandle BindTimerHandler;

};
