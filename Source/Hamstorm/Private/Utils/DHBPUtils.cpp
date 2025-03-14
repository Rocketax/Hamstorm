// Fill out your copyright notice in the Description page of Project Settings.

#include "Utils/DHBPUtils.h"

//This function fetches the steam language from the user. It is only used at the first launch of the game. After that,
//we override it with the user's setting in game.
FString UDHBPUtils::GetSteamLanguage()
{
	SteamAPI_RestartAppIfNecessary( 2944680 );
	
	if (SteamAPI_Init())
	{
		const char* Lang = SteamApps()->GetCurrentGameLanguage();
		return FString(Lang);
	}
	
	return FString(TEXT("Unknown"));
}

//This function unlocks a steam achievement.
void UDHBPUtils::UnlockAchievement(FString AchievementName)
{
	if (SteamAPI_Init())
	{
		if (SteamUserStats() != nullptr)
		{
			SteamUserStats()->SetAchievement(TCHAR_TO_ANSI(*AchievementName));
			SteamUserStats()->StoreStats();
		}
	}
}
