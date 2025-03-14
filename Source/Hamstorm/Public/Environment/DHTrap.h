// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/DHHoverableInterface.h"
#include "DHTrap.generated.h"

class UBoxComponent;

//Class for traps.
UCLASS()
class HAMSTORM_API ADHTrap : public AActor, public IDHHoverableInterface
{
	GENERATED_BODY()
	
public:	
	ADHTrap();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Trap")
	UStaticMeshComponent* StaticMeshComponent;

	virtual void ActivateTrap() PURE_VIRTUAL(ADHTrap::ActivateTrap);

	virtual void RearmTrap() PURE_VIRTUAL(ADHTrap::RearmTrap);
};
