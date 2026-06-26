// MGBombActor.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGBombActor.generated.h"

UCLASS()
class MINIGAMES_API AMGBombActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMGBombActor();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	// BombMesh를 부착할 위치
	UPROPERTY() 
	USceneComponent* Root;

	// BombActor의 Static Mesh
	UPROPERTY() 
	UStaticMeshComponent* BombMesh;

	// BombActor를 다른 사람에게 부착하기 위해 Collision 감지용 Sphere Component
	UPROPERTY() 
	class USphereComponent* PassTrigger;


};
