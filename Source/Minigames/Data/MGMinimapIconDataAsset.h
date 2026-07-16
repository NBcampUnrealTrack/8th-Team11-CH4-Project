// MGMinimapIconDataAsset.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Type/MGTypes.h"
#include "MGMinimapIconDataAsset.generated.h"

class UUserWidget;

UCLASS()
class MINIGAMES_API UMGMinimapIconDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Minimap Setup")
	TMap<EMinimapTargetType, TSubclassOf<UUserWidget>> IconMap;
};
