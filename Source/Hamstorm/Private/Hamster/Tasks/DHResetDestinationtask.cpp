// Fill out your copyright notice in the Description page of Project Settings.


#include "Hamster/Tasks/DHResetDestinationtask.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Game/DHGameState.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Hamster/DHHamsterController.h"
#include "Kismet/GameplayStatics.h"

//This function handles the end of the hamster's movement.
EBTNodeResult::Type UDHResetDestinationtask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(DestinationKey.SelectedKeyName, FAISystem::InvalidLocation);

	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	checkf(GameState, TEXT("Cannot find GameState"))

	GameState->OnMovementEnded.Broadcast();

	Cast<ADHHamsterCharacter>(Cast<ADHHamsterController>(OwnerComp.GetOwner())->GetPawn())->IsMoving = false;
	
	return EBTNodeResult::Succeeded;
}

void UDHResetDestinationtask::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}
