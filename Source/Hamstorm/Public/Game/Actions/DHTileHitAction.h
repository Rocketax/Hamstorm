// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/DHAction.h"
#include "DHTileHitAction.generated.h"

class ADHFloorTile;

//Class for tile hit action in a turn. Used to revert the weakening of a tile.
UCLASS()
class HAMSTORM_API UDHTileHitAction : public UDHAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Hit")
	ADHFloorTile* HitTile;
	
	virtual void RevertAction() override;
};
