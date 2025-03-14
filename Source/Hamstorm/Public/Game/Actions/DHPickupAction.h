// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/DHAction.h"
#include "DHPickupAction.generated.h"

class IDHPickupInterface;

//Class for a pickup action in a turn. Used to replace a pickup item such as a pickaxe or a collectible.
UCLASS()
class HAMSTORM_API UDHPickupAction : public UDHAction
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category="Pickup")
	AActor* PickupActor;

	UPROPERTY(BlueprintReadWrite, Category="Pickup")
	FVector Position;
	
	virtual void RevertAction() override;
};
