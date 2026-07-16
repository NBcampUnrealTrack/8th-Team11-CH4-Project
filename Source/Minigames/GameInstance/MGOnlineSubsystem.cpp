// MGOnlineSubsystem.cpp


#include "GameInstance/MGOnlineSubsystem.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"


void UMGOnlineSubsystem::Login()
{
	IOnlineSubsystem* OSS = Online::GetSubsystem(GetWorld());
	if (OSS == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[MGOnline] OnlineSubsystem null"));
		return;
	}
	
	IOnlineIdentityPtr Identity = OSS->GetIdentityInterface();
	if (Identity.IsValid() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("[MGOnline] Identity invalid"));
		return;
	}
	
	Identity->AddOnLoginCompleteDelegate_Handle(0, FOnLoginCompleteDelegate::CreateUObject(this, &UMGOnlineSubsystem::OnLoginComplete));

	FOnlineAccountCredentials Creds;
	Creds.Type = TEXT("accountportal");
	Creds.Id = TEXT("");
	Creds.Token = TEXT("");

	Identity->Login(0, Creds);
}

void UMGOnlineSubsystem::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("[MGOnline] EOS Login SUCCESS: %s"), *UserId.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[MGOnline] EOS Login FAILED: %s"), *Error);
	}
}

void UMGOnlineSubsystem::HostSession()
{
	IOnlineSubsystem* OSS = Online::GetSubsystem(GetWorld());
	if (OSS == nullptr)
	{
		return;
	}

	IOnlineSessionPtr Session = OSS->GetSessionInterface();
	if (Session.IsValid() == false)
	{
		return;
	}

	// 방 설정
	FOnlineSessionSettings Settings;
	Settings.bIsLANMatch            = false;   // EOS 릴레이 사용
	Settings.NumPublicConnections   = 4;       // 최대 인원
	Settings.bShouldAdvertise       = true;    // 목록에 노출
	Settings.bAllowJoinInProgress   = true;
	Settings.bUsesPresence          = true;
	Settings.bUseLobbiesIfAvailable = true;    // EOS 로비로 생성
	Settings.Set(SEARCH_KEYWORDS, FString("Minigames"),
				 EOnlineDataAdvertisementType::ViaOnlineService);   // 검색용 키워드

	Session->OnCreateSessionCompleteDelegates.AddUObject(
		this, &UMGOnlineSubsystem::OnCreateSessionComplete);
	Session->CreateSession(0, NAME_GameSession, Settings);
}

void UMGOnlineSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// 리슨 서버로 로비 맵 열기 → 호스트가 서버 겸 플레이어
		GetWorld()->ServerTravel(TEXT("/Game/Minigames/Level/L_Lobby?listen"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[MGOnline] CreateSession FAILED"));
	}
}

void UMGOnlineSubsystem::FindAndJoinSession()
{
	IOnlineSubsystem* OSS = Online::GetSubsystem(GetWorld());
	if (OSS == nullptr)
	{
		return;
	}
	IOnlineSessionPtr Session = OSS->GetSessionInterface();
	if (Session.IsValid() == false)
	{
		return;
	}

	SessionSearch = MakeShared<FOnlineSessionSearch>();
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->bIsLanQuery = false;
	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(SEARCH_KEYWORDS, FString("Minigames"), EOnlineComparisonOp::Equals);

	Session->OnFindSessionsCompleteDelegates.AddUObject(this, &UMGOnlineSubsystem::OnFindSessionsComplete);
	Session->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMGOnlineSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (bWasSuccessful == false || SessionSearch.IsValid() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MGOnline] FindSessions FAILED"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[MGOnline] Found %d sessions"), SessionSearch->SearchResults.Num());
	if (SessionSearch->SearchResults.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MGOnline] No sessions found"));
		return;
	}

	IOnlineSessionPtr Session = Online::GetSubsystem(GetWorld())->GetSessionInterface();
	Session->OnJoinSessionCompleteDelegates.AddUObject(this, &UMGOnlineSubsystem::OnJoinSessionComplete);
	Session->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[0]);   // 첫 방 참가
}

void UMGOnlineSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSessionPtr Session = Online::GetSubsystem(GetWorld())->GetSessionInterface();

	FString ConnectString;
	if (Session->GetResolvedConnectString(NAME_GameSession, ConnectString))
	{
		UE_LOG(LogTemp, Log, TEXT("[MGOnline] Join → travel: %s"), *ConnectString);
		if (APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController())
		{
			PC->ClientTravel(ConnectString, TRAVEL_Absolute);   // 호스트 맵으로 입장
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[MGOnline] Join: no connect string"));
	}
}
