// MGEffectDataAsset.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MGEffectDataAsset.generated.h"

UCLASS()
class MINIGAMES_API UMGEffectDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Name")
	FName EffectID = NAME_None;

	//-----------------------------

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Logic")
	float SpeedAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Logic")
	float Duration = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Logic")
	bool bCanStack = false;

	//-----------------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Visual")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Visual")
	class UTexture2D* BuffIcon;

	// TODO : Niagara Effect, Sound µî
};
