// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\Public\Enemies\DHMovingRatController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/DHMovingRat.h"
#include "Environment/DHDestructibleWall.h"
#include "Environment/DHFloorTile.h"
#include "Game/DHGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"

ADHMovingRatController::ADHMovingRatController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADHMovingRatController::SetDestination(FVector Destination)
{
	ADHMovingRat* MovingRat = Cast<ADHMovingRat>(GetPawn());
	MovingRat->IsMoving = true;
	
	//No destination set.
	if (Destination == FAISystem::InvalidLocation)
	{
		MovingRat->RevertTransform = MovingRat->GetActorTransform();
		
        checkf(Blackboard, TEXT("No blackboard component"))

		//Warning message to indicate path is not set. The path is set in the editor.
		if (MovingRat->TilesPath.IsEmpty())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("" + GetPawn()->GetName() + ": Path is not set"));
			
			return;
		}
    
		//Changing direction when at the end of the TilesPath array (the rat is at the end of the path, it needs to turn around).
        Direction = MovingRat->CurrentTile + Direction >= MovingRat->TilesPath.Num() ? -1
        : MovingRat->CurrentTile + Direction < 0 ? 1
		: Direction;

		//The tile isn't found. We stop the movement.
		if (!MovingRat->TilesPath.IsValidIndex(MovingRat->CurrentTile + Direction))
		{
			GEngine->AddOnScreenDebugMessage(-1,
				15,
				FColor::Red,
				FString::Printf(
					TEXT("%s tried going to index %d from array of size %d"),
					*MovingRat->GetName(),
					MovingRat->CurrentTile + Direction,
					MovingRat->TilesPath.Num()));
			Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()))->OnRatMovementEnded.Broadcast();
			Cast<ADHMovingRat>(GetPawn())->IsMoving = false;
			return;
		}
    
        FVector RaycastEnd = MovingRat->TilesPath[MovingRat->CurrentTile + Direction]->GetActorLocation();
    
        RaycastEnd.Z = MovingRat->GetActorLocation().Z * 0.6;
    
		//Raycast to check for obstacles in the way of the rat.
        FHitResult HitResult;
        FCollisionQueryParams QueryParams;

		QueryParams.AddIgnoredActors(MovingRats);
        QueryParams.AddIgnoredActors(Tiles);
		QueryParams.AddIgnoredActor(GetPawn());
    
        const bool IsHit = GetWorld()->LineTraceSingleByChannel(
        		HitResult,
        		MovingRat->GetActorLocation(),
        		RaycastEnd,
        		ECC_GameTraceChannel1,
        		QueryParams
        	);
    
		//If an obstacle is in the way, we change direction.
        if (IsHit && (HitResult.GetActor()->IsA(ADHDestructibleWall::StaticClass()) || HitResult.GetActor()->IsA(ADHRat::StaticClass())))
        {
        	Direction *= -1;
        }
    
        MovingRat->CurrentTile += Direction;

		//The tile isn't found. We stop the movement.
		if (!MovingRat->TilesPath.IsValidIndex(MovingRat->CurrentTile))
		{
			GEngine->AddOnScreenDebugMessage(-1,
				15,
				FColor::Red,
				FString::Printf(
					TEXT("%s tried going to index %d from array of size %d"),
					*MovingRat->GetName(),
					MovingRat->CurrentTile,
					MovingRat->TilesPath.Num()));
			Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()))->OnRatMovementEnded.Broadcast();
			Cast<ADHMovingRat>(GetPawn())->IsMoving = false;
			return;
		}
        
		//Set the movement destination.
        FVector ActualDestination = MovingRat->TilesPath[MovingRat->CurrentTile]->GetActorLocation();
        ActualDestination.Z = MovingRat->GetActorLocation().Z;

		MovingRat->AddMoveAction();
        
        GetBlackboardComponent()->SetValueAsVector(TEXT("Destination"), ActualDestination);
	}
	else
	{
		//Destination already set.
		GetBlackboardComponent()->SetValueAsVector(TEXT("Destination"), Destination);
	}
}

void ADHMovingRatController::RotateToNextTile()
{
	auto MovingRat = Cast<ADHMovingRat>(GetPawn());

	//Index of the tile to look at in the TilesPath array.
	int32 LookAtTile = 0;

	if (MovingRat->CurrentTile == 0)
	{
		LookAtTile = 1;
	}
	else if (MovingRat->CurrentTile == MovingRat->TilesPath.Num() - 1)
	{
		LookAtTile = MovingRat->CurrentTile - 1;
	}
	else
	{
		FVector RaycastEnd = MovingRat->TilesPath[MovingRat->CurrentTile + Direction]->GetActorLocation();
    
		RaycastEnd.Z = MovingRat->GetActorLocation().Z * 0.6;
    
		//Checking for obstacles.
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;

		QueryParams.AddIgnoredActors(MovingRats);
		QueryParams.AddIgnoredActors(Tiles);
		QueryParams.AddIgnoredActor(GetPawn());
    
		const bool IsHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				MovingRat->GetActorLocation(),
				RaycastEnd,
				ECC_GameTraceChannel1,
				QueryParams
			);

		if (IsHit)
		{
			Direction *= -1;
			LookAtTile = MovingRat->CurrentTile + Direction;
		}
		else
		{
			LookAtTile = MovingRat->CurrentTile + Direction;
		}
	}
	
	//Rotate the rat towards the next tile.
	auto Rotator = UKismetMathLibrary::FindLookAtRotation(GetPawn()->GetActorLocation(), MovingRat->TilesPath[LookAtTile]->GetActorLocation());

	MovingRat->SetActorRotation(FRotator(0, Rotator.Yaw, 0));
}

void ADHMovingRatController::BeginPlay()
{
	Super::BeginPlay();
	
	//Fetching required actors.
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHFloorTile::StaticClass(), Tiles);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHMovingRat::StaticClass(), MovingRats);
}
