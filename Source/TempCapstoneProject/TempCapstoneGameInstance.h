// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"

#include "Menus/MenuInterface.h"

#include "TempCapstoneGameInstance.generated.h"



USTRUCT()
struct FMapData
{
	GENERATED_BODY()

	UPROPERTY()
	FString MapName;

	UPROPERTY()
	FString MapPath;

	UPROPERTY()
		FString GameMode;

	UPROPERTY()
		FString GameModePath;
};

USTRUCT()
struct FGameModeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	 TSubclassOf<class AGameModeBase> GameMode;
	
	UPROPERTY()
	FString GameModeName;
};

USTRUCT(BlueprintType)
struct FFriendData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString RealName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString Presence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Online|Friend")
		FString UniqueNetId;
};

UCLASS()
class TEMPCAPSTONEPROJECT_API UTempCapstoneGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

public:
	UTempCapstoneGameInstance();

	void Init();

	//Join and Host
	UFUNCTION(Exec)
	virtual void Host(FString ServerName, FString Map) override;

	UFUNCTION(Exec)
	virtual void Join(uint32 Index) override;

	UFUNCTION(BlueprintCallable)
	void LoadMenuWidget();

	//Session Creation and Hosting
	void StartSession();
	virtual void LoadMainMenu() override;
	virtual void RefreshServerList() override;

	TSharedPtr<FMapData> MapToPlay;

	UPROPERTY(EditAnywhere, Category = "Game Modes")
	TArray<FGameModeData> GameModes;

	FORCEINLINE TArray<TSharedPtr<FMapData>> GetMapList(){ return MapList; }

	void SetMapToPlay(FString map);
	void SetGameModeToPlay(FString SelectedGameMode);

	UPROPERTY(EditDefaultsOnly, Category = "Player Characters")
		TSubclassOf<class ACharacter> Character1; 
	UPROPERTY(EditDefaultsOnly, Category = "Player Characters")
		TSubclassOf<class ACharacter> Character2;
	UPROPERTY(EditDefaultsOnly, Category = "Player Characters")
		int CharacterChoice = 0;

	void OnReadFriendsListComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);
	void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum, TSharedPtr<const FUniqueNetId> NetId, const FOnlineSessionSearchResult& SessionSearchResult);

	TArray<TSharedRef<FOnlineFriend>> FriendsList;
	TArray<FFriendData>FriendsArr;
	FOnReadFriendsListComplete OnReadFriendsListCompleteDelegate;
	FOnSessionUserInviteAcceptedDelegate OnSessionUserInviteAcceptedDelegate;

	void RetrieveFriendsList();
	bool InviteFriend(FString Name);

	IConsoleCommand* InviteFriendsConsoleCommand;

private:

	TSubclassOf<class UUserWidget> MenuClass;
	TSubclassOf<class UUserWidget> InGameMenuClass;

	class UMainMenu* Menu;

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	TArray<TSharedPtr<FMapData>> MapList;

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	FString DesiredServerName;
	FOnlineSessionSettings SessionSettings;
	void CreateSession();

	void ParseMaps();
	void SetGameModeToPlay(TSubclassOf<class AGameModeBase> gm);

};
