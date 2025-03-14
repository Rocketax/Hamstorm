// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Actions/DHMoveAction.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/DHDasherRat.h"
#include "Enemies/DHDasherRatController.h"
#include "Enemies/DHMovingRat.h"
#include "Enemies/DHMovingRatController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Hamster/DHHamsterController.h"

//This function restores the state of an actor that moved to what it was before it moved.
void UDHMoveAction::RevertAction()
{
	MovedActor->SetActorTransform(RevertTransform, false, nullptr, ETeleportType::ResetPhysics);

	//Hamster moved.
	if (MovedActor->IsA(ADHHamsterCharacter::StaticClass()))
	{
		ADHHamsterCharacter* HamsterCharacter = Cast<ADHHamsterCharacter>(MovedActor);
		HamsterCharacter->IsMoving = false;
		HamsterCharacter->GetCharacterMovement()->MovementMode = MOVE_Walking;

		ADHHamsterController* HamsterController = Cast<ADHHamsterController>(HamsterCharacter->GetController());
		HamsterController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanMove"), true);
	}

	//Rat moved.
	if (MovedActor->IsA(ADHRat::StaticClass())) Cast<ADHRat>(MovedActor)->IsMoving = false;

	//Moving rat moved.
	if (MovedActor->IsA(ADHMovingRat::StaticClass()))
	{
		ADHMovingRat* MovingRat = Cast<ADHMovingRat>(MovedActor);

		MovingRat->RevertTransform = RevertTransform;

		if (!IsAttacking) MovingRat->CurrentTile -= Direction;

		Cast<ADHMovingRatController>(MovingRat->GetController())->Direction = Direction;

		//The dasher rat has extra variables to reset.
		if (MovedActor->IsA(ADHDasherRat::StaticClass()))
		{
			ADHDasherRat* DasherRat = Cast<ADHDasherRat>(MovedActor);

			//The dasher hadn't moved, therefore hadn't seen hamstorm.
			if (DasherRat->CurrentTile == 0)
			{
				Cast<ADHDasherRatController>(DasherRat->GetController())->HasSeenHamstorm = false;
				DasherRat->OnHasSeenHamstorm.Broadcast(Cast<ADHDasherRatController>(DasherRat->GetController())->HasSeenHamstorm);
			}
			else
			{
				Cast<ADHDasherRatController>(DasherRat->GetController())->HasSeenHamstorm = true;
				DasherRat->OnHasSeenHamstorm.Broadcast(Cast<ADHDasherRatController>(DasherRat->GetController())->HasSeenHamstorm);
			}
		}
	}
	
	if (MovedActor->IsA(ADHRat::StaticClass()))
	{
		Cast<ADHRat>(MovedActor)->StopMovement();
	}
	
}
