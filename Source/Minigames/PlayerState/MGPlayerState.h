// MGPlayerState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Type/MGPlayerColor.h"
#include "MGPlayerState.generated.h"

/**
 *
 */
UCLASS()
class MINIGAMES_API AMGPlayerState : public APlayerState
{
	GENERATED_BODY()
public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EMGPlayerColor PlayerColor = EMGPlayerColor::Red;

    FLinearColor GetPlayerLinearColor() const;


};
