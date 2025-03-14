// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/DHAction.h"
#include "DHDestroyAction.generated.h"

//Class for a destroy action in a turn. Used to revert the destruction of walls and pillars.
UCLASS()
class HAMSTORM_API UDHDestroyAction : public UDHAction
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category="Destruction")
	AActor* DestroyedActor;

	UPROPERTY(BlueprintReadWrite, Category="Destruction")
	FVector OldPosition;
	
	virtual void RevertAction() override;
};
