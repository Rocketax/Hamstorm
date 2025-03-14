// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DHHoverableInterface.generated.h"

// Interface for hoverable objects (visual feedback). Implemented in blueprints.
UINTERFACE(MinimalAPI)
class UDHHoverableInterface : public UInterface
{
	GENERATED_BODY()
};

class HAMSTORM_API IDHHoverableInterface
{
	GENERATED_BODY()

public:
	
	bool IsHovered;
	
	UFUNCTION(BlueprintNativeEvent)
	void OnStartHovered();

	UFUNCTION(BlueprintNativeEvent)
	void OnStopHovered();
};
