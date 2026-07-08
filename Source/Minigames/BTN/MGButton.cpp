#include "MGButton.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Character.h" 
#include "Net/UnrealNetwork.h" 

AMGButton::AMGButton()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;

    // 충돌 박스 생성
    InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
    RootComponent = InteractionCollision;
    InteractionCollision->SetCollisionProfileName(TEXT("Trigger"));

    // 버튼 스태틱 메쉬 생성
    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    ButtonMesh->SetupAttachment(RootComponent);
    ButtonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMGButton::BeginPlay()
{
    Super::BeginPlay();

    CurrentOwnerState = nullptr;

    // 플레이어가 버튼 근처에 진입 시 호출할 함수
    InteractionCollision->OnComponentBeginOverlap.AddDynamic(this, &AMGButton::OnOverlapBegin);
    InteractionCollision->OnComponentEndOverlap.AddDynamic(this, &AMGButton::OnOverlapEnd);
}

// 플레이어가 근처에서 키를 누르면 호출될 함수 (소유권 변경)
bool AMGButton::SetButtonOwner(APlayerState* NewOwnerState)
{
    // 유효성 검사
    if (!NewOwnerState)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ 실패: NewOwnerState가 nullptr입니다!"));
        return false;
    }

    // 시스템 고유 ID 비교
    if (CurrentOwnerState != nullptr)
    {
        if (CurrentOwnerState->GetUniqueId() == NewOwnerState->GetUniqueId())
        {
            UE_LOG(LogTemp, Warning, TEXT("❌ 무시: 이미 이 버튼의 진짜 주인입니다. (ID 일치)"));
            return false;
        }
    }

    if (!HasAuthority())
    {
        Server_SetButtonOwner(NewOwnerState);
        return true;
    }


    // 이전 소유권자 개수 감소
    if (CurrentOwnerState != nullptr)
    {
        float OldScore = CurrentOwnerState->GetScore();
        CurrentOwnerState->SetScore(FMath::Max(0.f, OldScore - 1.f));
    }

    // 새 소유권자 개수 증가
    NewOwnerState->SetScore(NewOwnerState->GetScore() + 1.f);

    // 소유권 이전
    CurrentOwnerState = NewOwnerState;

    // 성공 로그 출력 
    UE_LOG(LogTemp, Warning, TEXT("성공 버튼: %s ➔ 새 주인: %s"),
        *GetName(), *NewOwnerState->GetPlayerName());

    // 버튼 색상 변경 이벤트 호출 (서버 기준)
    OnButtonColorChanged(NewOwnerState);

    return true;
}

// 서버 RPC
void AMGButton::Server_SetButtonOwner_Implementation(APlayerState* NewOwnerState)
{
    SetButtonOwner(NewOwnerState);
}

// Replication
void AMGButton::OnRep_CurrentOwnerState()
{
    GetWorldTimerManager().SetTimerForNextTick([this]()
        {
            OnButtonColorChanged(CurrentOwnerState);
        });
}

void AMGButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMGButton, CurrentOwnerState);
}

// 충돌 진입
void AMGButton::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ACharacter* OverlappedCharacter = Cast<ACharacter>(OtherActor))
    {
        UE_LOG(LogTemp, Log, TEXT("%s 가 버튼 범위에 들어왔습니다!"), *OverlappedCharacter->GetName());
    }
}

// 충돌 종료
void AMGButton::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (ACharacter* OverlappedCharacter = Cast<ACharacter>(OtherActor))
    {
        UE_LOG(LogTemp, Log, TEXT("%s 가 버튼 범위를 벗어났습니다."), *OverlappedCharacter->GetName());
    }
}
