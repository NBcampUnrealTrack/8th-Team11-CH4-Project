#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gimmick/Button/MGInteractable.h"
#include "MGButtonActor.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;

UCLASS()
class MINIGAMES_API AMGButtonActor : public AActor, public IMGInteractable
{
    GENERATED_BODY()
public:
    AMGButtonActor();

protected:
    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    virtual void BeginInteract_Implementation(AActor* Interactor) override;
    virtual void EndInteract_Implementation(AActor* Interactor) override;

private:
    void ApplyVisual();

    UFUNCTION()
    void OnRep_Pressed();

    UFUNCTION()
    void OnRep_Color();

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

    UPROPERTY(ReplicatedUsing = OnRep_Pressed)
    bool bPressed = false;

    UPROPERTY(ReplicatedUsing = OnRep_Color)
    FLinearColor CurrentColor = FLinearColor::Black;
};