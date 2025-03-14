// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Tasks/DHRatMoveEndTask.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/DHMovingRat.h"
#include "Enemies/DHMovingRatController.h"
#include "Game/DHGameState.h"
#include "Kismet/GameplayStatics.h"

EBTNodeResult::Type UDHRatMoveEndTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	checkf(GameState, TEXT("Cannot find GameState"))

	GameState->OnRatMovementEnded.Broadcast();

	//Resetting variables to end movement and prepare for the next one.
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(DestinationKey.SelectedKeyName, FAISystem::InvalidLocation);

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("CanMove"), false);

	auto RatController = Cast<ADHMovingRatController>(OwnerComp.GetOuter());

	checkf(RatController, TEXT("Not a rat controller"))

	RatController->RotateToNextTile();
	
	// Storing the current transform for revert.
	Cast<ADHMovingRat>(RatController->GetPawn())->RevertTransform = RatController->GetPawn()->GetActorTransform();

	Cast<ADHRat>(RatController->GetPawn())->IsMoving = false;
	
	return EBTNodeResult::Succeeded;
}

void UDHRatMoveEndTask::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}
