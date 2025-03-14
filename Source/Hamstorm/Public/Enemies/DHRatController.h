// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DHRatController.generated.h"

//Class for rat AI Controller
UCLASS()
class HAMSTORM_API ADHRatController : public AAIController
{
	GENERATED_BODY()

public:

	ADHRatController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	UBehaviorTree* BehaviorTree;

	// The default value is the value of FAISystem::InvalidLocation
	UFUNCTION(BlueprintCallable, Category="AI")
	virtual void SetDestination(FVector Destination = FVector(3.402823466e+38F));
};
