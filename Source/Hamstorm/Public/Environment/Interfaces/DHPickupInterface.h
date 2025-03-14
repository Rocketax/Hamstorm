// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DHPickupInterface.generated.h"

class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickup);

//Interface for pickable objects.
UINTERFACE(BlueprintType)
class HAMSTORM_API UDHPickupInterface : public UInterface
{
	GENERATED_BODY()
};

class HAMSTORM_API IDHPickupInterface
{
	GENERATED_BODY()
	
public:

	virtual bool TryPickup() = 0;
	
	virtual void AddPickupAction() = 0;
};
