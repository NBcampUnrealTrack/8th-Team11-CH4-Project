// MGOnlineSubsystem.h

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MGOnlineSubsystem.generated.h"

UCLASS()
class MINIGAMES_API UMGOnlineSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void Login();
	void HostSession();
	void FindAndJoinSession();
	
private:
	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;  
};
