// MGMinimapTargetComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MGMinimapTargetComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINIGAMES_API UMGMinimapTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMGMinimapTargetComponent();

protected:
	virtual void BeginPlay() override;

private:
	void RegisterToMinimap();

protected:
	UPROPERTY(EditAnywhere, Category = "Minimap")
	TSubclassOf<UUserWidget> MyPlayerCharacterIcon;

	UPROPERTY(EditAnywhere, Category = "Minimap")
	TSubclassOf<UUserWidget> OtherPlayerCharacterIcon;

};
