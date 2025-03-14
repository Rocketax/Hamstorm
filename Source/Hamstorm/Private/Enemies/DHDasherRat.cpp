// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/DHDasherRat.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/DHDasherRatController.h"
#include "Game/DHGameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ADHDasherRat::ADHDasherRat()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADHDasherRat::StopMovement(bool ShouldEndTurn)
{
	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));
	
	ADHDasherRatController* DasherRatController = Cast<ADHDasherRatController>(GetController());

	GetCharacterMovement()->Velocity = FVector(0, 0, 0);

	//Resetting blackboard variables.
	DasherRatController->GetBlackboardComponent()->SetValueAsVector(TEXT("Destination"), FAISystem::InvalidLocation);

	DasherRatController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanMove"), false);

	if (ShouldEndTurn) GameState->OnRatMovementEnded.Broadcast();
}

void ADHDasherRat::BeginPlay()
{
	Super::BeginPlay();
}

