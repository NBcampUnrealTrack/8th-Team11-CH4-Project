#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MGInteractable.generated.h"

UINTERFACE(MinimalAPI)
class UMGInteractable : public UInterface
{
	GENERATED_BODY()
};

class MINIGAMES_API IMGInteractable
{
	GENERATED_BODY()

	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BeginInteract(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EndInteract(AActor* Interactor);
};