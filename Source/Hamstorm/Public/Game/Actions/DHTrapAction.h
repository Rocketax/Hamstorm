// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/DHAction.h"
#include "DHTrapAction.generated.h"

class ADHPressurePlate;

//Class for a trap action in a turn. Used to revert the activation of a trap.
UCLASS()
class HAMSTORM_API UDHTrapAction : public UDHAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Trap")
	ADHPressurePlate* PressurePlate;
	
	virtual void RevertAction() override;
};
