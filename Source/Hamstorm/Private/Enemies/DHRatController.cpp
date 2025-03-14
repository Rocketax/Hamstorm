// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/DHRatController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/DHRat.h"

ADHRatController::ADHRatController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADHRatController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviorTree);
}

void ADHRatController::SetDestination(FVector Destination)
{
	Cast<ADHRat>(GetPawn())->IsMoving = true;
	
	auto* BlackboardComponent = GetBlackboardComponent();

	checkf(BlackboardComponent, TEXT("No blackboard"))

	BlackboardComponent->SetValueAsVector(TEXT("Destination"), Destination);
}

