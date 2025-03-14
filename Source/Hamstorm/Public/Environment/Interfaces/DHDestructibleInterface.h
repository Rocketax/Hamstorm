// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DHDestructibleInterface.generated.h"

class ADHFloorTile;

// Interface for destructible objects.
UINTERFACE(Blueprintable)
class UDHDestructibleInterface : public UInterface
{
	GENERATED_BODY()
};

class IDHDestructibleInterface
{
	GENERATED_BODY()

public:

	virtual void SetShouldBeDestroyed(bool ShouldBeDestroyed) = 0;
	
	virtual void Destruct() = 0;

	virtual ADHFloorTile* CheckValidAdjacentTile() = 0;
};
