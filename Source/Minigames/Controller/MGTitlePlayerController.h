// MGTitlePlayerController.h

#pragma once

#include "GameFramework/PlayerController.h"
#include "MGTitlePlayerController.generated.h"

/**
 *
 */
UCLASS()
class MINIGAMES_API AMGTitlePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	void HostGame();
	
	void JoinGame();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ASUIPlayerController, Meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> UIWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ASUIPlayerController, Meta = (AllowPrivateAccess))
	TObjectPtr<UUserWidget> UIWidgetInstance;

};
