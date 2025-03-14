// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/DHAction.h"
#include "DHKillAction.generated.h"

class ADHRat;

//Class for a kill action in a turn. Used to revert the kill of rats or the hamster.
UCLASS()
class HAMSTORM_API UDHKillAction : public UDHAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Killed")
	ACharacter* KilledActor;

	UPROPERTY(BlueprintReadWrite, Category="Killed")
	FVector OldPosition;
	
	virtual void RevertAction() override;
};
