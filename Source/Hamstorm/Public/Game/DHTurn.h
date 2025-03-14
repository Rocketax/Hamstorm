// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DHTurn.generated.h"

class UDHAction;

//Class for every turn.
UCLASS()
class HAMSTORM_API UDHTurn : public UObject
{
	GENERATED_BODY()

public:

	UDHTurn();
	
	UPROPERTY(BlueprintReadWrite, Category="Actions")
	TArray<UDHAction*> Actions;

	UFUNCTION(BlueprintCallable, Category="Actions")
	void Revert();
	
};
