// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/DHAction.h"
#include "DHRatInvasionAction.generated.h"

//Class for a rat invasion action in a turn. Used to despawn all rats coming from a rat invasion.
UCLASS()
class HAMSTORM_API UDHRatInvasionAction : public UDHAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Spawn")
	TArray<AActor*> SpawnedActors;
	
	virtual void RevertAction() override;
	
};
