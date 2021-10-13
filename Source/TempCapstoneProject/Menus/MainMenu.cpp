// Fill out your copyright notice in the Description page of Project Settings.


#include "../Menus/MainMenu.h"

#include "Components/Button.h"
#include "../TempCapstoneGameInstance.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitalizer)
{
    
}

//void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
//{
//}
//
void UMainMenu::SelectIndex(uint32 Index)
{
    SelectedIndex = Index;
}

bool UMainMenu::Initialize()
{
    bool Success = Super::Initialize();
    if (!Success) return false;


    if (!ensure(HostButton != nullptr)) return false;
    HostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

    if(!ensure(JoinButton != nullptr)) return false;
    JoinButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

    return true;
}

void UMainMenu::HostServer()
{
    if (MenuInterface != nullptr)
    {
        FString ServerName = "Test";
        FString Map = "Map1";
        MenuInterface->Host(ServerName, Map);
    }
}

void UMainMenu::JoinServer()
{
    if (MenuInterface != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected Index: %d"), 0);
        MenuInterface->Join(0);
    }
}

//void UMainMenu::OpenHostMenu()
//{
//}
//
//void UMainMenu::OpenJoinMenu()
//{
//}

//void UMainMenu::OpenMainMenu()
//{
//}

//void UMainMenu::MapSelect(FString SelectedItem, ESelectInfo::Type SelectionType)
//{
//}

//void UMainMenu::UpdateChildren()
//{
//}
