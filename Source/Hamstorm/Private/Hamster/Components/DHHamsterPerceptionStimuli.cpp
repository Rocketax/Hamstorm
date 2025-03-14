// Fill out your copyright notice in the Description page of Project Settings.


#include "Hamster/Components/DHHamsterPerceptionStimuli.h"

UDHHamsterPerceptionStimuli::UDHHamsterPerceptionStimuli(const FObjectInitializer& ObjectInitializer)
		: UAIPerceptionStimuliSourceComponent(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;

	bAutoRegisterAsSource = true;
}

void UDHHamsterPerceptionStimuli::BeginPlay()
{
	Super::BeginPlay();
}
