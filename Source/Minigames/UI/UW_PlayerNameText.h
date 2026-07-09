// UW_PlayerNameText.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_PlayerNameText.generated.h"

class UTextBlock;

UCLASS()
class MINIGAMES_API UUW_PlayerNameText : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetOwningActor(AActor* InOwningActor) { OwningActor = InOwningActor; }
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> OwningActor;

private:
	FString LastName;
};
