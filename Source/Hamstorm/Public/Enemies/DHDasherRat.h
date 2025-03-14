// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DHMovingRat.h"
#include "DHRat.h"
#include "GenericTeamAgentInterface.h"
#include "DHDasherRat.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHasSeenHamstorm, bool, HasSeenHamstorm);

//Class for the dasher rat enemy. It's a moving rat that only starts moving when it sees the hamster. It moves 2 tiles at a time.
UCLASS()
class HAMSTORM_API ADHDasherRat : public ADHMovingRat
{
	GENERATED_BODY()

public:

	ADHDasherRat();
	
	virtual void StopMovement(bool ShouldEndTurn = true) override;

	UPROPERTY(BlueprintAssignable, Category="Vision")
	FOnHasSeenHamstorm OnHasSeenHamstorm;

protected:

	virtual void BeginPlay() override;
};
