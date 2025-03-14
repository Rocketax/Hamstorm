// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Actions/DHKillAction.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/DHMovingRat.h"
#include "Enemies/DHRat.h"
#include "Game/DHGameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Hamster/DHHamsterController.h"
#include "Kismet/GameplayStatics.h"

//This function restores the state of the killed actor to what it was before being killed.
void UDHKillAction::RevertAction()
{
	FNavLocation NavLocation;
	
	KilledActor->SetActorLocation(OldPosition, false, nullptr, ETeleportType::ResetPhysics);
	
	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(KilledActor->GetWorld()));

	//A rat was killed
	if (KilledActor->IsA(ADHRat::StaticClass()))
	{
		Cast<ADHRat>(KilledActor)->IsDead = false;
		GameState->NbRatsKilled--;
		Cast<ADHRat>(KilledActor)->IsFalling = false;
		Cast<ADHRat>(KilledActor)->HasAlreadyDied = false;
	}

	//A moving rat was killed
	if (KilledActor->IsA(ADHMovingRat::StaticClass()))
	{
		Cast<ADHMovingRat>(KilledActor)->GetCharacterMovement()->SetActive(true);
		Cast<ADHMovingRat>(KilledActor)->IsFalling = false;
	}

	//The hamster was killed
	if (KilledActor->IsA(ADHHamsterCharacter::StaticClass()))
	{
		ADHHamsterCharacter* HamsterCharacter = Cast<ADHHamsterCharacter>(KilledActor);

		HamsterCharacter->IsDead = false;

		HamsterCharacter->IsFalling = false;
		
		HamsterCharacter->GetCharacterMovement()->SetActive(true);

		ADHHamsterController* HamsterController = Cast<ADHHamsterController>(HamsterCharacter->GetController());
		HamsterController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanMove"), true);
	}

}
