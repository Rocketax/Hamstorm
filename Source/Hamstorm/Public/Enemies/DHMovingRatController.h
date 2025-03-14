// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DHRatController.h"
#include "DHMovingRatController.generated.h"


//Class for moving rat AI Controller
UCLASS()
class HAMSTORM_API ADHMovingRatController : public ADHRatController
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<AActor*> Tiles;

	UPROPERTY()
	TArray<AActor*> MovingRats;

public:

	ADHMovingRatController();

	//This variable is used to indicate in which direction to traverse the TilesPath array. 1 is forward and -1 is backwards.
	//This is because the rat moves back and forth.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Pathing")
	int32 Direction = 1;

	// The default value is the value of FAISystem::InvalidLocation
	virtual void SetDestination(FVector Destination = FVector(3.402823466e+38F)) override;

	void RotateToNextTile();
protected:
	virtual void BeginPlay() override;
	
};
