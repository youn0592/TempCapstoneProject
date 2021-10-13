// Fill out your copyright notice in the Description page of Project Settings.


#include "TempCapstoneGameInstance.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "JsonUtilities.h"
#include "Menus/MenuWidget.h"

#include "OnlineSubsystemNames.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSharingInterface.h"

#include "Menus/MainMenu.h"

const static FName SESSION_NAME = TEXT("Game");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

/*
THINGS TO RESEARCH

TSharedRef
TSharedPtr
int32

The Online Sub class.
*/

//
UTempCapstoneGameInstance::UTempCapstoneGameInstance()
{
    //TODO
    ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
    if (!ensure(MenuBPClass.Class != nullptr)) return; 
    MenuClass = MenuBPClass.Class;

    //TODO
    //ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Content/MenuSystem/WBP_InGameMenu"));
    //if (!ensure(InGameMenuBPClass.Class != nullptr)) return;
    //InGameMenuClass = InGameMenuBPClass.Class;

    InviteFriendsConsoleCommand = IConsoleManager::Get().RegisterConsoleCommand(TEXT("InviteFriend"), TEXT("test"), FConsoleCommandWithArgsDelegate::CreateLambda(
        [&](const TArray< FString >& Args)
        {
            FString FriendName;
            for (FString Arg : Args)
            {
                FriendName += Arg;
            }
            InviteFriend(FriendName);
        }
    ), ECVF_Default);
}
//
void UTempCapstoneGameInstance::Init()
{
    Super::Init();

    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Found Subsystem %s"), *Subsystem->GetSubsystemName().ToString());

        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid()) {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UTempCapstoneGameInstance::OnCreateSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UTempCapstoneGameInstance::OnDestroySessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UTempCapstoneGameInstance::OnFindSessionComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UTempCapstoneGameInstance::OnJoinSessionComplete);

            OnReadFriendsListCompleteDelegate = FOnReadFriendsListComplete::CreateUObject(this, &UTempCapstoneGameInstance::OnReadFriendsListComplete);
            OnSessionUserInviteAcceptedDelegate = FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UTempCapstoneGameInstance::OnSessionUserInviteAccepted);

            SessionInterface->AddOnSessionUserInviteAcceptedDelegate_Handle(OnSessionUserInviteAcceptedDelegate);
        }
    }
    else 
    {
        UE_LOG(LogTemp, Warning, TEXT("Found No Subsystem"));
    }

    ParseMaps();
}
//
void UTempCapstoneGameInstance::Host(FString ServerName, FString Map)
{
    DesiredServerName = ServerName;

    if (SessionInterface.IsValid())
    {
        auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
        if (ExistingSession != nullptr)
        {
            SessionInterface->DestroySession(SESSION_NAME);
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Host Created"));
            CreateSession();
        }
    }
}
//
void UTempCapstoneGameInstance::Join(uint32 Index)
{
    if (!SessionInterface.IsValid()) return;
    if (!SessionSearch.IsValid()) return;

    if (Menu != nullptr)
    {
        Menu->Teardown();
    }

    SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);

}
//
void UTempCapstoneGameInstance::StartSession()
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->StartSession(SESSION_NAME);
    }
}
//
void UTempCapstoneGameInstance::LoadMainMenu()
{
    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if (!ensure(PlayerController != nullptr)) return;
    PlayerController->ClientTravel("/Menus/MainMenu", ETravelType::TRAVEL_Absolute);
}

void UTempCapstoneGameInstance::LoadMenuWidget()
{
    if (!ensure(MenuClass != nullptr)) return;

    Menu = CreateWidget<UMainMenu>(this, MenuClass);
    if (!ensure(Menu != nullptr)) return;

    Menu->Setup();

    Menu->SetMenuInterface(this);
}

//
void UTempCapstoneGameInstance::RefreshServerList()
{
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    if (SessionSearch.IsValid())
    {
        if (SessionSettings.bIsLANMatch)
        {
            SessionSearch->bIsLanQuery = true;
        }
        else
        {
            SessionSearch->bIsLanQuery = false;
        }

        SessionSearch->MaxSearchResults = 100;
        SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
        UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));
        SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
    }
}
//
void UTempCapstoneGameInstance::SetMapToPlay(FString map)
{
    for (int i = 0; i < MapList.Num(); i++)
    {
        if (map == MapList[i]->MapName)
        {
            MapToPlay = MapList[i];
            break;
        }
    }
}
//
void UTempCapstoneGameInstance::SetGameModeToPlay(TSubclassOf<class AGameModeBase> gm)
{
    if (MapToPlay)
    {
        MapToPlay->GameMode = gm->GetName();
        MapToPlay->GameModePath = gm->GetPathName();
    }
}
//
void UTempCapstoneGameInstance::SetGameModeToPlay(FString SelectedGameMode)
{
    for (int i = 0; i < GameModes.Num(); i++)
    {
        if (GameModes[i].GameModeName.Equals(SelectedGameMode))
        {
            SetGameModeToPlay(GameModes[i].GameMode);
        }
    }
}
//
void UTempCapstoneGameInstance::OnReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
    if (bWasSuccessful)
    {
        auto Friends = Online::GetFriendsInterface();
        if (Friends.IsValid())
        {
            FriendsArr.Empty();

            ULocalPlayer* Player = Cast<ULocalPlayer>(GetWorld()->GetFirstLocalPlayerFromController());

            Friends->GetFriendsList(LocalUserNum, ListName, FriendsList);
            for (int32 i = 0; i < FriendsList.Num(); i++)
            {
                TSharedRef<FOnlineFriend> Friend = FriendsList[i];
                FFriendData FriendData = FFriendData();

                FriendData.Presence = EOnlinePresenceState::ToString(Friend->GetPresence().Status.State);
                FriendData.DisplayName = Friend->GetDisplayName();
                FriendData.RealName = Friend->GetRealName();
                FriendData.UniqueNetId = Friend->GetUserId()->ToString();
                FriendsArr.Add(FriendData);

                UE_LOG_ONLINE_FRIEND(Warning, TEXT("Friends: %s"), *FriendData.DisplayName);
                GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Black, "FRIEND = " + FriendData.DisplayName);
            }
        }
    }
}
//
void UTempCapstoneGameInstance::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum, TSharedPtr<const FUniqueNetId> NetId, const FOnlineSessionSearchResult& SessionSearchResult)
{
    GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("OnSessionUserInviteAccepted: %d"), bWasSuccessful));

    if (bWasSuccessful)
    {
        if (SessionSearchResult.IsValid())
        {
            IOnlineSessionPtr SessionInt = IOnlineSubsystem::Get()->GetSessionInterface();
            SessionInt->JoinSession(LocalUserNum, SESSION_NAME, SessionSearchResult);
        }
    }

}
//
void UTempCapstoneGameInstance::RetrieveFriendsList()
{
    auto Friends = Online::GetFriendsInterface();
    if (Friends.IsValid())
    {
        ULocalPlayer* Player = Cast<ULocalPlayer>(GetWorld()->GetFirstLocalPlayerFromController());
        Friends->ReadFriendsList(Player->GetControllerId(), EFriendsLists::ToString(EFriendsLists::Default), OnReadFriendsListCompleteDelegate);
    }
} 
//
bool UTempCapstoneGameInstance::InviteFriend(FString Name)
{
    IOnlineSessionPtr Sessions = Online::GetSessionInterface();
    IOnlineFriendsPtr Friends = Online::GetFriendsInterface();
    TArray<TSharedRef<FOnlineFriend>> OutFriends;

    Friends->GetFriendsList(0, EFriendsLists::ToString(EFriendsLists::Default), OutFriends);
    TSharedPtr<const FUniqueNetId, ESPMode::NotThreadSafe> FriendId;

    for (int32 i = 0; i < OutFriends.Num(); i++)
    {
        TSharedRef<FOnlineFriend> Friend = OutFriends[i];
        UE_LOG_ONLINE_FRIEND(Warning, TEXT("Friend: %s"), *Friend->GetUserId()->ToString());
        //Add Include
        GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Black, "Friend = " + Friend->GetUserId()->ToString());

        if (Friend->GetRealName() == Name)
        {
            FriendId = Friend->GetUserId();
        }

           }
     const ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
     const FUniqueNetId& CFriendId = *FriendId.Get();
        
     return Sessions->SendSessionInviteToFriend(LocalPlayer->GetControllerId(), SESSION_NAME, CFriendId);
} 
//
void UTempCapstoneGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
    if (!Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could Not Create Session"));
        GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, TEXT("Session Creation Failed"));
        return;
    }
    
     if(Menu != nullptr)
     {
        Menu->Teardown();
     }
    

    UEngine* Engine = GetEngine();
    if (!ensure(Engine != nullptr)) return;

    Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

    UWorld* World = GetWorld();
    if (!ensure(World != nullptr)) return;

    FString LobbyMap = "/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen";
    if (!SessionSettings.bIsLANMatch)
    {
        RetrieveFriendsList();
    }
    
    World->ServerTravel(LobbyMap);

    if (!SessionSettings.bIsLANMatch)
    {
        RetrieveFriendsList();

    }

    
}
//
void UTempCapstoneGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
    if (Success)
    {
        CreateSession();
    }
}
//
void UTempCapstoneGameInstance::OnFindSessionComplete(bool Success)
{
    auto Results = SessionSearch->SearchResults;
    if (Success && SessionSearch.IsValid() && Menu != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Finished Find Sessions"));

        TArray<FServerData> ServerNames;
        for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
        {
            UE_LOG(LogTemp, Warning, TEXT("Found session names: %s"), *SearchResult.GetSessionIdStr());
            FServerData Data;
            Data.Name = SearchResult.GetSessionIdStr();
            Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
            Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
            Data.HostUsername = SearchResult.Session.OwningUserName;
            FString ServerName;
            if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
            {
                Data.Name = ServerName;
            }
            else
            {
                Data.Name = "Could Not Find Name";
            }
            ServerNames.Add(Data);
        }
        //Menu->SetServerList(ServerNames);
    }
}
//
void UTempCapstoneGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (!SessionInterface.IsValid()) return;

    FString Address;
    if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not get connect string."));
        return;
    }

    UEngine* Engine = GetEngine();
    if (!ensure(Engine != nullptr)) return;

    Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if (!ensure(PlayerController != nullptr)) return;

    PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}
//
void UTempCapstoneGameInstance::CreateSession()
{
    if (SessionInterface.IsValid())
    {
        FName name = IOnlineSubsystem::Get()->GetSubsystemName();
        if (name.IsEqual("NULL"))
        {
            SessionSettings.bIsLANMatch = true;
        }
        else
        {
            SessionSettings.bIsLANMatch = false;
        }
        SessionSettings.NumPublicConnections = 5;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.bUsesPresence = true;
        SessionSettings.bAllowJoinInProgress = true;
        SessionSettings.bAllowJoinViaPresence = true;
        SessionSettings.bUseLobbiesIfAvailable  = true;
        SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Session Created"));

        SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
    }
}
//
void UTempCapstoneGameInstance::ParseMaps()
{
    const FString JsonFilePath = FPaths::ProjectConfigDir() + "/JsonFiles/maps.json";
    FString JsonString; //Json Converted to FString

    FFileHelper::LoadFileToString(JsonString, *JsonFilePath);

    GLog->Log("Json String");
    GLog->Log(JsonString);

    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
    {
        TSharedPtr<FJsonObject> MapObject = JsonObject->GetObjectField("GameMaps");

        TArray<TSharedPtr<FJsonValue>> objArray = MapObject->GetArrayField("maps");
        GLog->Log("Printing Maps Names...");
        for (int32 index = 0; index < objArray.Num(); index++)
        {
            TSharedPtr<FJsonObject> map = objArray[index]->AsObject();
            GLog->Log("name:" + map->GetStringField("name"));

            TSharedPtr<FMapData> MapData = MakeShared<FMapData>();
            MapData->MapName = map->GetStringField("name");

            TArray<TSharedPtr<FJsonValue>> mapsArray = map->GetArrayField("info");
            MapData->MapPath = mapsArray[0]->AsString();

            for (int32 index2 = 0; index2 < mapsArray.Num(); index2++)
            {
                GLog->Log("		info:" + mapsArray[index2]->AsString());
            }
            MapList.AddUnique(MapData);
        }
    }

    else
    {
        GLog->Log("Couldn't Deserialize");
    }

}

