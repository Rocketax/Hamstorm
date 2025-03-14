// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "DHHamsterPerceptionStimuli.generated.h"

//Class for the stimuli associated with the perception component.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HAMSTORM_API UDHHamsterPerceptionStimuli : public UAIPerceptionStimuliSourceComponent
{
	GENERATED_BODY()

public:

	UDHHamsterPerceptionStimuli(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void BeginPlay() override;

};
