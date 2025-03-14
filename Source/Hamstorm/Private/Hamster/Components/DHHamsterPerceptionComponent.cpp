// Fill out your copyright notice in the Description page of Project Settings.


#include "Hamster/Components/DHHamsterPerceptionComponent.h"

#include "Enemies/DHRat.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Hamster/DHHamsterController.h"

UDHHamsterPerceptionComponent::UDHHamsterPerceptionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDHHamsterPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();

	OnTargetPerceptionUpdated.AddDynamic(this, &UDHHamsterPerceptionComponent::OnTargetPerception);
	
}

void UDHHamsterPerceptionComponent::OnTargetPerception(AActor* Actor, FAIStimulus Stimulus)
{
	//A rat is in front of the hamster.
	if (Actor->IsA(ADHRat::StaticClass()))
	{
		ADHHamsterController* Controller = Cast<ADHHamsterController>(GetOuter());
    
        checkf(Controller, TEXT("No Controller"))
    
        ADHHamsterCharacter* Character = Cast<ADHHamsterCharacter>(Controller->GetPawn());
    
        checkf(Character, TEXT("No Character"))
		
		Character->LastSensedActor = Actor;

		if (!Character->IsMoving) Character->OnEnemyVisionUpdated.Broadcast();
	}
}

