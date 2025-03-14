// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/DHDasherRatController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/DHDasherRat.h"
#include "Environment/DHFloorTile.h"
#include "Game/DHGameState.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Kismet/GameplayStatics.h"

ADHDasherRatController::ADHDasherRatController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADHDasherRatController::SetDestination(FVector Destination)
{
	ADHDasherRat* DasherRat = Cast<ADHDasherRat>(GetPawn());
	DasherRat->IsMoving = true;
	
	//Destination already set.
	if (Destination != FAISystem::InvalidLocation)
	{
		GetBlackboardComponent()->SetValueAsVector(TEXT("Destination"), Destination);
	}
	//Destination isn't set.
	else
	{
		TArray<AActor*> Rats;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHRat::StaticClass(), Rats);

		DasherRat->RevertTransform = DasherRat->GetActorTransform();

		//Check for hamster in LOS (Line Of Sight)
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(Tiles);
		QueryParams.AddIgnoredActors(Rats);
		QueryParams.AddIgnoredActor(GetPawn());
		QueryParams.AddIgnoredActor(this);

		FHitResult HitResult;

		FVector EndDestination = GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector() * 10000;
		EndDestination.Z = GetPawn()->GetActorLocation().Z / 2;

		GetWorld()->LineTraceSingleByChannel(
			HitResult,
			GetPawn()->GetActorLocation(),
			EndDestination,
			ECC_Pawn,
			QueryParams
		);

		//Actor seen. Setting the variable HasSeenHamstorm accordingly.
		if (HitResult.GetActor() && !HasSeenHamstorm)
		{
			UE_LOG(LogTemp, Warning, TEXT("DASHER: Seen %s"), *HitResult.GetActor()->GetName())
			HasSeenHamstorm = HitResult.GetActor()->IsA(ADHHamsterCharacter::StaticClass());
			DasherRat->OnHasSeenHamstorm.Broadcast(HasSeenHamstorm);
		}
	
		//Set destination and start moving
		if (HasSeenHamstorm)
		{
			checkf(Blackboard, TEXT("No blackboard component"))
    
			//Changing direction when reaching the end of the path
			Direction = DasherRat->CurrentTile + 1 >= DasherRat->TilesPath.Num() ? -1
			: DasherRat->CurrentTile - 1 < 0 ? 1
			: Direction;

			DasherRat->CurrentTile += Direction;

			checkf(DasherRat->TilesPath.Num() > 0, TEXT("No tiles in TilePath of %s"), *DasherRat->GetName())
			checkf(DasherRat->TilesPath[DasherRat->CurrentTile], TEXT("Set the TilePath of %s"), *DasherRat->GetName())
        
			//Set the destination to the next tile
			FVector ActualDestination = DasherRat->TilesPath[DasherRat->CurrentTile]->GetActorLocation();
			ActualDestination.Z = DasherRat->GetActorLocation().Z;

			DasherRat->AddMoveAction();
		
			GetBlackboardComponent()->SetValueAsVector(TEXT("Destination"), ActualDestination);

			DasherRat->OnHasSeenHamstorm.Broadcast(HasSeenHamstorm);

			//Reset variables when returning to the starting point.
			if (DasherRat->CurrentTile == 0)
			{
				HasSeenHamstorm = false;
				DasherRat->OnHasSeenHamstorm.Broadcast(HasSeenHamstorm);
			}
		}
		//Hasn't seen the hamster, it doesn't move.
		else
		{
			auto* GameState = Cast<ADHGameState>(GetWorld()->GetGameState());

			checkf(GameState, TEXT("Cannot find GameState"))

			GameState->OnRatMovementEnded.Broadcast();

			GetBlackboardComponent()->SetValueAsBool(TEXT("CanMove"), false);
		}
	}
}

void ADHDasherRatController::BeginPlay()
{
	Super::BeginPlay();
	
	//Fetching required actors
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHFloorTile::StaticClass(), Tiles);
}

