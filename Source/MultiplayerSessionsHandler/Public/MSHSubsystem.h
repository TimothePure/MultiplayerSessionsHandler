// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MSHSubsystem.generated.h"

// Custom delegates for games classes to bind callbacks to
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMSHOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMSHOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful); 
DECLARE_MULTICAST_DELEGATE_OneParam(FMSHOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMSHOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMSHOnStartSessionComplete, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONSHANDLER_API UMSHSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UMSHSubsystem();

	// To Handle session functionality, menus and other classes will call these

	void CreateSession(int32 NumConnections, const FString& MatchType, bool bIsPrivate = false);
	void FindSessions(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession();
	void StartSession();

	// Custom delegates declaration
	FMSHOnCreateSessionComplete MSHOnCreateSessionComplete;
	FMSHOnFindSessionsComplete MSHOnFindSessionsComplete;
	FMSHOnJoinSessionComplete MSHOnJoinSessionComplete;
	FMSHOnDestroySessionComplete MSHOnDestroySessionComplete;
	FMSHOnStartSessionComplete MSHOnStartSessionComplete;
	
protected:
	// Internal callbacks for the delegates we'll add to the OnlineSessionInterface delegate list
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	
private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	// To add to the Session Intefarce delegate list
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;

	bool bCreateSessionOnDestroy{false};
	bool bLastIsPrivate;
	int32 LastNumConnections;
	FString LastMatchType;
};
