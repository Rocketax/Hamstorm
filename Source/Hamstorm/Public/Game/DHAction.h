// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DHAction.generated.h"

//Generic class for an action in a turn. There is no logic in that specific class.
UCLASS()
class HAMSTORM_API UDHAction : public UObject
{
	GENERATED_BODY()

public:

	UDHAction();
	
	virtual void RevertAction() PURE_VIRTUAL(UDHAction::RevertAction);
};
