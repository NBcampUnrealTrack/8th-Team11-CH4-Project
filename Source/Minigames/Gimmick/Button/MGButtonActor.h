#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Component/Button/MGInteractable.h"
#include "MGButtonActor.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;

UCLASS()
class MINIGAMES_API AMGButtonActor: public AActor, public IMGInteractable
{
    GENERATED_BODY()
public:

    AMGButtonActor();

protected:

    virtual void BeginPlay() override;

public:

    virtual void BeginInteract_Implementation(AActor* Interactor) override;
    virtual void EndInteract_Implementation(AActor* Interactor) override;

private:

    void ApplyVisual();

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> BaseMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> ButtonMesh;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

    UPROPERTY(EditAnywhere, Category = "Button")
    float PressDepth = 20.f;

    UPROPERTY(EditAnywhere, Category = "Button")
    FName ColorParameterName = TEXT("Color");

private:

    FVector ButtonOriginLocation;

    bool bPressed = false;

    FLinearColor CurrentColor = FLinearColor::Black;
};