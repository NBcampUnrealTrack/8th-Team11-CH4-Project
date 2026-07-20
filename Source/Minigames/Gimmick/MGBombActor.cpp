// MGBombActor.cpp

#include "Gimmick/MGBombActor.h"
#include "Minigames.h"						// Log 매크로 
#include "Component/MGStatusComponent.h"	// PlayerCharacter의 Status -> HP, Stamina 등 이용
#include "Components/SphereComponent.h"		// 폭탄을 넘길 Sphere Collision
#include "GameFramework/Character.h"		// Character
#include "Net/UnrealNetwork.h"				// Replication
#include "Components/CapsuleComponent.h"	// Socket이 없을 때 CapsuleComponent의 중앙으로 Attach
#include "GameMode/MGPassBombGameMode.h"		// Explode를 GameMode에 알려줘야함
#include "Character/MGPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MGPassBombHUD.h"				// Bomb HUD

#include "DrawDebugHelpers.h"				// Debug용

AMGBombActor::AMGBombActor() :
	PassTriggerRadius(100.f),
	BombHolder(nullptr),
	PassCooldownTime(0.5f),
	bCanPass(true)
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

	SetActorHiddenInGame(true);
	PassTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMGBombActor::BeginPlay()
{
	Super::BeginPlay();
	
	// 서버에서만 충돌을 감지하도록 연결
	if (HasAuthority())
	{
		PassTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMGBombActor::OnTriggerOverlap);
	}

	// Dedicated Server - Client 구조에서 0번 PC = 플레이어 본인
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PC))
	{
		AMGPassBombHUD* BombHUD = Cast<AMGPassBombHUD>(PC->GetHUD());
		if (IsValid(BombHUD))	// 서버는 HUD가 없기 때문에 캐스팅 실패, 클라이언트만 실행됨
		{
			BombHUD->BindWithBombActor(this);
		}
	}
}

// Replication에 필요한 기본 함수
void AMGBombActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 현재 MGBombActor를 가지고 있는 Character 포인터
	DOREPLIFETIME(AMGBombActor, BombHolder);
	DOREPLIFETIME(AMGBombActor, BombRemainTime);
}

void AMGBombActor::OnTriggerOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, 
	const FHitResult& SweepResult)
{
	if (bShowDebug)	// Debug가 켜져있으면
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
	
	AMGPlayerCharacter* OverlappedCharacter = Cast<AMGPlayerCharacter>(OtherActor);

	// Overlapped된 Character이고 && 현재 폭탄을 들고 있지 않다면
	if (OverlappedCharacter && OverlappedCharacter != BombHolder)
	{
		MG_LOG_ROLE(LogMGNet, Warning, TEXT("Bomb Passed [%s] -> [%s]"),
			BombHolder ? *BombHolder->GetName() : TEXT("Initial Point"),
			*OverlappedCharacter->GetName());

		SetBombHolder(OverlappedCharacter);		// 폭탄을 Overlapped된 Character에 부착
	}
	else
	{
		return;
	}
}

// 폭탄을 NewHolder에 부착, OnTriggerOverlap에서 실행됨
void AMGBombActor::SetBombHolder(AMGPlayerCharacter* NewHolder)
{
	if (!HasAuthority() || BombHolder == NewHolder)
	{
		return;	// Authority가 없거나 || NewHolder와 (현재)BombHolder가 같다면 조기종료
	}

	if (IsValid(BombHolder))
	{
		BombHolder->MulticastRPC_SetCarryState(false);
	}
	BombHolder = NewHolder; // BombHolder 값 변경 시, 레플리케이션으로 클라이언트들에 OnRep_BombHolder() 자동 호출
	OnRep_BombHolder();	// OnRep 함수가 AttachToHolder 이외에 다른 기능이 추가됨에 따라 직접 호출로 변경
	BombHolder->MulticastRPC_SetCarryState(true);

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

	if (OnBombHolderChanged.IsBound())
	{	// 클라이언트 UI 등에 Broadcast
		OnBombHolderChanged.Broadcast(BombHolder);
	}
}

void AMGBombActor::TickBombTimer()
{
	if (BombRemainTime > 0)
	{
		--BombRemainTime;			// BombRemainTime 값 변경 = 클라이언트에서 OnRep 함수 실행
		OnRep_BombRemainTime();		// 서버는 직접 호출
	}

	// 0초가 되면 Clear Timer
	if (BombRemainTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(BombCountdownTimerHandler);
	}
}

void AMGBombActor::SetBombCollision(bool Value)
{
	if (!HasAuthority())
	{
		return;
	}
	PassTrigger->SetCollisionEnabled(Value ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

	// 물리월드와 게임월드는 병렬로 돌아감. 이때 특정한 상황에서 SetCollisionEnabled를 실행해도 두 월드의 충돌로 적용되지 않는 현상이 발생할 수 있음.
	// 그래서 다음 틱에 오버랩을 업데이트하는 것임.
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
		PassTrigger->UpdateOverlaps(nullptr, true);
		}
	);

	//디버깅 코드
	/*
	if (PassTrigger->GetCollisionEnabled())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Collision: True"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Collision: False"));
	}
	*/
}

void AMGBombActor::OnRep_BombRemainTime()
{
	if (OnBombTimeChanged.IsBound())
	{
		OnBombTimeChanged.Broadcast(BombRemainTime);
	}
}

// Timer가 있다면 반드시 EndPlay에서 안전하게 ClearTimer 로직 추가
void AMGBombActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(ExplodeTimer);
	GetWorld()->GetTimerManager().ClearTimer(PassCooldownTimer);

	// 상속받은 EndPlay의 Super는 마지막에
	Super::EndPlay(EndPlayReason);
}

void AMGBombActor::AttachToHolder(AMGPlayerCharacter* TargetHolder)
{
	if (!TargetHolder)	// TargetHolder가 유효하지 않으면
	{
		return;
	}

	USkeletalMeshComponent* MeshComp = TargetHolder->GetMesh();
	UCapsuleComponent* CapsuleComp = TargetHolder->GetCapsuleComponent();

	if (!IsValid(MeshComp) || !IsValid(CapsuleComp))
	{
		return;
	}

	// 변수 'AttachSocketName'이 NAME_Nome이 아니고 (= 에디터에서 BombActor의 AttachSocketName에 값 입력)
	// && 스켈레탈 메쉬가 존재하고 && 'AttachSocketName'변수 이름의 소켓이 실제로 존재할 때
	if (!AttachSocketName.IsNone() && MeshComp->DoesSocketExist(AttachSocketName))
	{
		this->AttachToComponent(
			MeshComp,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			AttachSocketName		// 변수 'AttachSocketName' 소켓에 부착
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

void AMGBombActor::ActivateBomb(AMGPlayerCharacter* InitialHolder, float ExplodeTime)
{
	if (!HasAuthority())
	{
		return;
	}

	SetBombHolder(InitialHolder);
	SetActorHiddenInGame(false);
	SetBombCollision(false);

	BombRemainTime = FMath::CeilToInt(ExplodeTime);
	OnRep_BombRemainTime();		// 서버도 최초 갱신

	// 1초마다 TickBombTimer 함수를 반복(true) 실행하는 타이머 작동
	GetWorldTimerManager().SetTimer(
		BombCountdownTimerHandler,
		this,
		&AMGBombActor::TickBombTimer,
		1.0f,
		true
	);

	GetWorldTimerManager().SetTimer(
		ExplodeTimer,
		this,
		&AMGBombActor::ExplodeBomb,
		ExplodeTime,
		false
	);

	if (bShowDebug)
	{
		FString DebugMessage = FString::Printf(TEXT("[%s] Countdown begin : %.1f"),
			(GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("Client%02d"), UE::GetPlayInEditorID()) : ((GetNetMode() == ENetMode::NM_Standalone) ? TEXT("StandAlone") : TEXT("Server")),
			ExplodeTime);

		UE_LOG(LogMGNet, Warning, TEXT("%s"), *DebugMessage);
		//GEngine->AddOnScreenDebugMessage(
		//	-1,                 // 고유 Key (-1은 기존 메시지를 지우지 않고 계속 새로 쌓음)
		//	5.0f,               // 화면에 메시지가 머무르는 시간 (5초)
		//	FColor::Cyan,       // 글자 색상
		//	DebugMessage        // 출력할 문자열
		//);
	}
}

// 서버에서만 실행되는 로직
void AMGBombActor::ExplodeBomb()
{
	if (!HasAuthority())
	{
		return;
	}	// 서버에서만 실행되도록 Authority 체크 후 조기 종료

	MG_LOG_ROLE(LogMGNet, Warning, TEXT("Bomb explode : %s"),
		BombHolder ? *BombHolder->GetName() : TEXT("Initial Point"));

	SetActorHiddenInGame(true);
	SetBombCollision(false);

	if (BombHolder)
	{
		AMGPassBombGameMode* CurrentGameMode = Cast<AMGPassBombGameMode>(GetWorld()->GetAuthGameMode());
		if (CurrentGameMode)
		{
			// 탈락 처리 함수를 호출하면서 현재 폭탄 주인을 인자로 넘기기 
			CurrentGameMode->EliminatePlayer(BombHolder);
		}
	}

	// 모든 클라이언트에 Multicast, [폭탄 나이아가라 이펙트, 사운드] 등
	Multicast_OnExplode();

	// 따로 RPC를 설정하지 않아도 자동으로 레플리케이션
	// Destroy();			// Multicast 함수 호출 직후에 Destroy를 할 경우 패킷이 보내지지 않을 수 있음
	// SetLifeSpan(0.1f);		// Multicast가 될 수 있도록 약간의 딜레이 후 Destroy

	BombHolder = nullptr;		// BombHolder 초기화
}

void AMGBombActor::Multicast_OnExplode_Implementation()
{
	MG_LOG_NET(LogMGNet, Log, TEXT("Explosion Niagara Effect and Sound"));
	// TODO : 나이아가라 이펙트 생성, 사운드 재생 등
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, GetActorLocation(), GetActorRotation(), FVector::OneVector * ExplosionScale);
}
