// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/DHMovingRat.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/DHMovingRatController.h"
#include "Game/DHGameState.h"
#include "Game/DHTurn.h"
#include "Game/Actions/DHMoveAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void ADHMovingRat::BeginPlay()
{
	Super::BeginPlay();

	RevertTransform = GetActorTransform();
}

void ADHMovingRat::Die(bool ShouldStopMovement)
{
	if (HasAlreadyDied || Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()))->IsReverting) return;

	//Disable the movement component when dying.
	GetCharacterMovement()->SetActive(false);
	
	Super::Die();
}

void ADHMovingRat::StopMovement(bool ShouldEndTurn)
{
	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));
	
	ADHMovingRatController* MovingRatController = Cast<ADHMovingRatController>(GetController());

	GetCharacterMovement()->Velocity = FVector(0, 0, 0);

	//Resetting blackboard variables.
	MovingRatController->GetBlackboardComponent()->SetValueAsVector(TEXT("Destination"), FAISystem::InvalidLocation);

	MovingRatController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanMove"), false);

	if (ShouldEndTurn) GameState->OnRatMovementEnded.Broadcast();
}

//This function stores the necessary data to revert a movement in a move action.
void ADHMovingRat::AddMoveAction()
{
	UDHMoveAction* MoveAction = NewObject<UDHMoveAction>();

	MoveAction->MovedActor = this;

	MoveAction->RevertTransform = RevertTransform;

	MoveAction->IsAttacking = IsAttacking;

	MoveAction->Direction = Cast<ADHMovingRatController>(GetController())->Direction;

	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	//Add the action to the current turn.
	GameState->Turns[GameState->Turns.Num() - 1]->Actions.Push(MoveAction);
}

