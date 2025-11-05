// Fill out your copyright notice in the Description page of Project Settings.


#include "MSHMenu.h"

#include "Components/Button.h"
#include "MSHSubsystem.h"
#include "OnlineSessionSettings.h"

void UMSHMenu::MenuSetup(int32 NumberOfConnections , FString TypeOfMatch, FString LobbyPath)
{
	NumConnections = NumberOfConnections;
	MatchType = TypeOfMatch;
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		MSHSubsystem = GameInstance->GetSubsystem<UMSHSubsystem>();
	}

	// Bind MSHSubsystem callbacks
	if (MSHSubsystem)
	{
		MSHSubsystem->MSHOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MSHSubsystem->MSHOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MSHSubsystem->MSHOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MSHSubsystem->MSHOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MSHSubsystem->MSHOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

bool UMSHMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
	
	return true;
}

void UMSHMenu::NativeDestruct()
{
	MenuTearDown();
	
	Super::NativeDestruct();
}

void UMSHMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString(TEXT("Session created successfully"))
				);
		}
		if (UWorld* World = GetWorld())
		{
			World->ServerTravel(PathToLobby);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create session"));
		HostButton->SetIsEnabled(true);
	}
}

void UMSHMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (!MSHSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Sessions could not be found because the MSHSubsystem is null."));
		return;
	}
	
	for (auto Result : SessionResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;

		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			MSHSubsystem->JoinSession(Result);
			return;
		}
	}

	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMSHMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);
			
			if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
			{
				PlayerController->ClientTravel(Address, TRAVEL_Absolute);
			}
		}
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMSHMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMSHMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMSHMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if (MSHSubsystem)
	{
		MSHSubsystem->CreateSession(NumConnections, MatchType, false);
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("Session could not be created because the MSHSubsystem is null."))
	}
}

void UMSHMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if (MSHSubsystem)
	{
		MSHSubsystem->FindSessions(10000.f);
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("Session could not be joined because the MSHSubsystem is null."))
	}
}

void UMSHMenu::MenuTearDown()
{
	RemoveFromParent();
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
