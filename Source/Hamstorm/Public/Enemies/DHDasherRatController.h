// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DHMovingRatController.h"
#include "DHRatController.h"
#include "DHDasherRatController.generated.h"

//Class for dasher rat AI Controller
UCLASS()
class HAMSTORM_API ADHDasherRatController : public ADHMovingRatController
{
	GENERATED_BODY()

public:

	ADHDasherRatController();

	// The default value is the value of FAISystem::InvalidLocation
	virtual void SetDestination(FVector Destination = FVector(3.402823466e+38F)) override;

	UPROPERTY()
	bool HasSeenHamstorm = false;

protected:

	virtual void BeginPlay() override;
};
