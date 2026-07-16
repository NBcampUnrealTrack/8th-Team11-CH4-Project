#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gimmick/Button/MGInteractable.h"
#include "MGButtonActor.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class APlayerState;
class USoundBase;
class USoundAttenuation;

UCLASS()
class MINIGAMES_API AMGButtonActor : public AActor, public IMGInteractable
{
	GENERATED_BODY()

public:
	AMGButtonActor();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void BeginInteract_Implementation(AActor* Interactor) override;
	virtual void EndInteract_Implementation(AActor* Interactor) override;

	float GetButtonTopWorldZ() const;

	UFUNCTION(BlueprintPure, Category = "Gameplay|Button")
	APlayerState* GetCurrentOwner() const { return CurrentOwnerState; }
	float GetWorldPressDepth() const;

private:
	void ApplyVisual();

	bool SetButtonOwner(APlayerState* NewOwnerState);

	void PlayButtonSound(bool bNowPressed);

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

	UPROPERTY(EditAnywhere, Category = "Button|Sound")
	TObjectPtr<USoundBase> PressSound;

	UPROPERTY(EditAnywhere, Category = "Button|Sound")
	TObjectPtr<USoundAttenuation> ButtonSoundAttenuation;

	UPROPERTY(EditAnywhere, Category = "Button|Sound", meta = (ClampMin = "0.0"))
	float PressSoundCooldown = 0.3f;

private:
	FVector ButtonOriginLocation;

	FTimerHandle NextTickVisualHandle;
	FTimerHandle ReapplyVisualHandle;

	UPROPERTY(ReplicatedUsing = OnRep_Pressed)
	bool bPressed = false;

	UPROPERTY(ReplicatedUsing = OnRep_Color)
	FLinearColor CurrentColor;

	UPROPERTY(Replicated)
	bool bColorOverridden = false;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Gameplay|Button")
	TObjectPtr<APlayerState> CurrentOwnerState = nullptr;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayPressSound();

	float LastPressSoundTime = -100.f;
};