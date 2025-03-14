// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "DHHamsterPerceptionComponent.generated.h"

//Component to detect rats in front of the hamster. It is used to start an animation where the hamster prepares to attack.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HAMSTORM_API UDHHamsterPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()

public:

	UDHHamsterPerceptionComponent();

protected:

	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void OnTargetPerception(AActor* Actor, FAIStimulus Stimulus);

};
