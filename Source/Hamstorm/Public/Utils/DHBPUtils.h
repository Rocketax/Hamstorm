// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#pragma warning(push)
#pragma warning(disable:4996)

#include "Hamstorm/Steam/steam_api.h"

#pragma warning(pop)

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DHBPUtils.generated.h"

//This class is used to expose different functions that are implemented in C++ in blueprints.
UCLASS()
class HAMSTORM_API UDHBPUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Steam")
	static FString GetSteamLanguage();

	UFUNCTION(BlueprintCallable, Category="Steam")
	static void UnlockAchievement(FString AchievementName);
	
};
