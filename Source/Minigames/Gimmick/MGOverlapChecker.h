// MGOverlapChecker.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGOverlapChecker.generated.h"

class UBoxComponent;
class UMGEffectDataAsset;

UCLASS()
class MINIGAMES_API AMGOverlapChecker : public AActor
{
	GENERATED_BODY()
	
public:	
	AMGOverlapChecker();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void CheckOverlappingActors();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Overlap")
	TArray<AActor*> CurrentOverlappingActors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	UBoxComponent* OverlapBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
	const UMGEffectDataAsset* ItemEffectData;

private:
	FTimerHandle OverlapTimerHandler;

};
