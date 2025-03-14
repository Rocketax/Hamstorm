// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/DHAction.h"
#include "DHTeleportAction.generated.h"

class ADHPipe;

//Class for a teleport action in a turn. Used to revert the use of a pipe.
UCLASS()
class HAMSTORM_API UDHTeleportAction : public UDHAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Teleport")
	AActor* TeleportedActor;

	UPROPERTY(BlueprintReadWrite, Category="Teleport")
	ADHPipe* InitialPipe;
	
	UPROPERTY(BlueprintReadWrite, Category="Teleport")
	FTransform OldTransform;
	
	virtual void RevertAction() override;
};
