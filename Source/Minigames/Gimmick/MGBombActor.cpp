// MGBombActor.cpp

#include "Gimmick/MGBombActor.h"
#include "Minigames.h"						// Log 매크로 
#include "Component/MGStatusComponent.h"	// PlayerCharacter의 Status -> HP, Stamina 등 이용
#include "Components/SphereComponent.h"		// 폭탄을 넘길 Sphere Collision
#include "GameFramework/Character.h"		// Character
#include "Net/UnrealNetwork.h"				// Replication
#include "Components/CapsuleComponent.h"	// Socket이 없을 때 CapsuleComponent의 중앙으로 Attach

#include "DrawDebugHelpers.h"				// Debug용

AMGBombActor::AMGBombActor() :
	PassTriggerRadius(100.f),
	bCanPass(true),
	PassCooldownTime(0.5f),
	BombHolder(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombStaticMesh"));
	BombMesh->SetupAttachment(RootComponent);
	BombMesh->SetCollisionProfileName(TEXT("NoCollision")); // 폭탄 자체의 StaticMesh는 NoCollision

	PassTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("PassTrigger"));
	PassTrigger->SetupAttachment(RootComponent);
	PassTrigger->SetSphereRadius(PassTriggerRadius);		// PassTriggerRadius 값 변경 가능

	bReplicates = true;

	AttachSocketName = NAME_None;
}

void AMGBombActor::BeginPlay()
{
	Super::BeginPlay();
	
	// 서버에서만 충돌을 감지하도록 연결
	if (HasAuthority())
	{
		PassTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMGBombActor::OnTriggerOverlap);
	}
}

void AMGBombActor::OnTriggerOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, 
	const FHitResult& SweepResult)
{

	if (bShowDebugSphere)	// Debug가 켜져있으면
	{
		DrawDebugSphere(
			GetWorld(),
			GetActorLocation(),             // 구체의 중심점 (폭탄의 현재 위치)
			PassTriggerRadius,              // 구체의 반지름
			16,                             // 세그먼트 수
			FColor::Red,                    // 선 색상
			false,                          // 영구 지속 여부 (false면 특정 시간 뒤 사라짐)
			1.5f,                           // 지속 시간
			0,                              // 깊이 우선순위, 0순위면 어떠한 경우에도 보임
			1.5f                            // 선 두께
		);
	}

	if (!HasAuthority() || !bCanPass)
	{
		return;
	}	// Authority가 없거나 Pass를 할 수 없다면 조기 종료

	// 범용 Character로 Cast
	ACharacter* OverlappedCharacter = Cast<ACharacter>(OtherActor);

	// Overlapped된 Character이고 && 현재 폭탄을 들고 있지 않다면
	if (OverlappedCharacter && OverlappedCharacter != BombHolder)
	{
		MG_LOG_ROLE(LogTemp, Warning, TEXT("Bomb Passed [%s] -> [%s]"),
			BombHolder ? *BombHolder->GetName() : TEXT("Initial Point"),
			*OverlappedCharacter->GetName());

		SetBombHolder(OverlappedCharacter);		// 폭탄을 Overlapped된 Character에 부착
	}
}

// 폭탄을 NewHolder에 부착, OnTriggerOverlap에서 실행됨
void AMGBombActor::SetBombHolder(ACharacter* NewHolder)
{
	if (!HasAuthority() || BombHolder == NewHolder)
	{
		return;	// Authority가 없거나 || NewHolder와 (현재)BombHolder가 같다면 조기종료
	}

	BombHolder = NewHolder;
	AttachToHolder(NewHolder);

	// 폭탄을 옮겼으면 
	bCanPass = false;						// 폭탄을 들고 있지 않기 때문에 false
	GetWorldTimerManager().SetTimer(
		PassCooldownTimer,					// 관리하는 Handler 객체
		this,								// 이 객체에
		&AMGBombActor::ResetPassCooldown,   // 타이머 종료시 실행될 함수, bCanPass를 true로 만들어주는 함수
		PassCooldownTime,					// 쿨타임
		false								// 반복 여부, false : 반복x
	);
}

// BombHolder 변수 값이 바뀌었을 때 모든 Client들에 실행되는 함수
void AMGBombActor::OnRep_BombHolder()
{
	AttachToHolder(BombHolder);
}

// Replication에 필요한 기본 함수
void AMGBombActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 현재 MGBombActor를 가지고 있는 Character 포인터
	DOREPLIFETIME(ThisClass, BombHolder);
}

void AMGBombActor::AttachToHolder(ACharacter* TargetHolder)
{
	if (!TargetHolder)	// TargetHolder가 유효하지 않으면
	{
		return;
	}

	USkeletalMeshComponent* MeshComp = TargetHolder->GetMesh();
	UCapsuleComponent* CapsuleComp = TargetHolder->GetCapsuleComponent();

	// 변수 'AttachSocketName'이 NAME_Nome이 아니라면 (= 에디터에서 BombActor의 AttachSocketName에 값 입력)
	if (!AttachSocketName.IsNone() && MeshComp && MeshComp->DoesSocketExist(AttachSocketName))
	{
		this->AttachToComponent(
			MeshComp,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			AttachSocketName		// 변수 'AttachSocketName'에 부착
		);
	}
	else // 변수 'AttachSocketName'이 NAME_None 이거나 (= 에디터에서 BombActor의 AttachSocketName에 값 입력 안했을 때)
	{	 // 에디터에서 입력한 'AttachSocketName'이름의 소켓이 없을 때
		this->AttachToComponent(
			CapsuleComp,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale
		);	// CapsuleComponent의 정중앙에 부착
	}
}

void AMGBombActor::ResetPassCooldown()
{
	bCanPass = true;
}
