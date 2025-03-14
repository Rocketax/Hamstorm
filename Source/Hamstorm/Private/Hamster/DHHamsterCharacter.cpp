// Fill out your copyright notice in the Description page of Project Settings.


#include "Hamster/DHHamsterCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "Enemies/DHRat.h"
#include "Engine/StaticMeshActor.h"
#include "Environment/DHDestructibleWall.h"
#include "Environment/DHFloorTile.h"
#include "Environment/DHPickaxe.h"
#include "Environment/Interfaces/DHPickupInterface.h"
#include "Game/DHGameState.h"
#include "Game/DHTurn.h"
#include "Game/Actions/DHKillAction.h"
#include "Game/Actions/DHMoveAction.h"
#include "Hamster/DHHamsterController.h"
#include "Hamster/Components/DHHamsterPerceptionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DHPlayerController.h"

class ADHHamsterController;

//This function checks for what tiles are reachable from the hamster's location in all 4 directions.
void ADHHamsterCharacter::CheckPossibleTiles()
{
	IsTileChosen = false;
	
	//The current tile is set in the editor. It has to be set to start.
	if (!CurrentTile)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hamster: CurrentTile is not set"));
		
		return;
	}

	//We check if a rat is perceived directly in front of the hamster.
	IsRatInLoS();

	auto* PlayerController = Cast<ADHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	
	checkf(PlayerController, TEXT("No valid playercontroller"))
	
	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	checkf(GameState, TEXT("Cannot find GameState"))

	//There is queued movement, meaning the player clicked further than a single tile.
	if (QueuedMovement.Peek())
	{
		//Movement can't happen with these conditions.
		if (PlayerController->IsInDialog || PlayerController->IsGamePaused) return;

		AActor* Tile;
		QueuedMovement.Dequeue(Tile);

		GameState->IsQueuedMovement = true;

		PlayerController->MoveCharacter(Tile);
		return;
	}
	
	FVector StartingPoint = GetActorLocation();
	StartingPoint.Z -= 25;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(TArray<AActor*>{CurrentTile, this});
	TArray<FDHDirectionLength> PossibleTilesNumber;

	PossibleTiles.Empty();
	PossibleDestroyableWalls.Empty();
	
	//Raycast in all 4 directions to detect what tiles are accessible.
	for (int i = 0; i < 4; i++)
	{
		TArray<ADHFloorTile*> HitTiles;
		
		FVector Direction = GetActorForwardVector().RotateAngleAxis(90 * i, FVector(0, 0, 1));

		bool HasHit = false;

		bool IsFirstRaycast = true;

		FDHDirectionLength DirectionLength;
		DirectionLength.Length = 0.f;

		//We do another loop since the raycast only goes 1 tile. We need to raycast until we don't hit anything to make
		//sure that we got all the tiles in that direction.
		do {
        	HasHit = GetWorld()->LineTraceSingleByChannel(
                HitResult,
                StartingPoint,
                StartingPoint  + Direction * RaycastDistance,
                ECC_GameTraceChannel1,
                QueryParams
            );
    
            if (HitResult.GetActor())
            {
            	//Raycast hit a tile.
	            if (HitResult.GetActor()->IsA(ADHFloorTile::StaticClass()))
	            {
	            	auto Tile = Cast<ADHFloorTile>(HitResult.GetActor());

	            	checkf(Tile, TEXT("Not a floortile"))

	            	if (Tile->Destructible)
	            	{
	            		if (IsFirstRaycast)
	            		{
							//We want to weaken a destructible tile when it is the tile right next to the hamster if it was clicked.
	            			if (Tile->ShouldBeDestroyed && !GameState->IsReverting) PlayerController->OnDestroyWall.Broadcast(Tile); 

	            			else PossibleDestroyableWalls.Add(Tile);
	            		}
	            	}
	            	else
	            	{
						HitTiles.Add(Tile);
	            		IsFirstRaycast = false;
	            	}
	            }
				//Raycast hit a wall.
            	if (HitResult.GetActor()->IsA(ADHDestructibleWall::StaticClass()))
            	{
            		ADHDestructibleWall* Wall = Cast<ADHDestructibleWall>(HitResult.GetActor());
					if (IsFirstRaycast)
					{
						//We want to destroy the wall when it is on the tile right next to the hamster if it was clicked.
						if (Wall->ShouldBeDestroyed && !GameState->IsReverting) PlayerController->OnDestroyWall.Broadcast(Wall); 

						else PossibleDestroyableWalls.Add(HitResult.GetActor());
					}
					//We didn't hit a tile, we can change direction.
            		HasHit = false;
            	}
				//We hit the environment.
            	if (HitResult.GetActor()->IsA(AStaticMeshActor::StaticClass()))
            	{
            		HasHit = false;
            	}
	            QueryParams.AddIgnoredActor(HitResult.GetActor());
            	
            }
		} while (HasHit == true);

		for (auto HitTile : HitTiles)
		{
			PossibleTiles.Add(HitTile);
			/*
			 *	Event for possible tile for highlighting
			 *	Left: X: 0, Y: -1, Z: 0 -> 2
			 *	Front: X: 1, Y: 0, Z: 0 -> 0
			 *	Right: X: 0, Y: 1, Z: 0 -> 3
			 *	Back: X: -1, Y: 0, Z: 0 -> 1
			 */

			/*
			 *	Why static_cast<int32>(FMath::RoundHalfFromZero(static_cast<float>(value))?
			 *	Just casting in int32 would sometimes make it equal to 1 or 0 depending on the case.
			 *	When casting to a float, it transforms the 1 into 0.999866 and it becomes 0 when casting back to an int32.
			 *	To solve that, we have to round it before casting it to a float.
			 */

			int32 PossibleTile = static_cast<int32>(FMath::RoundHalfFromZero(static_cast<float>(Direction.X))) != 0 ?
				(
					static_cast<int32>(FMath::RoundHalfFromZero(static_cast<float>(Direction.X))) == 1 ?
					0:
					1
				):
				(
					static_cast<int32>(FMath::RoundHalfFromZero(static_cast<float>(Direction.Y))) == 1 ?
					3:
					2
				);

			DirectionLength.Direction = PossibleTile;
			DirectionLength.Length += 1.f;
		}
		if (DirectionLength.Length > 0) PossibleTilesNumber.Add(DirectionLength);
	}
	OnPossibleTile.Broadcast(PossibleTilesNumber);
}

//This function stores the necessary data to revert a movement in a move action.
void ADHHamsterCharacter::AddMoveAction()
{
	UDHMoveAction* MoveAction = NewObject<UDHMoveAction>();

	MoveAction->MovedActor = this;

	MoveAction->RevertTransform = GetActorTransform();

	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	//Add the action to the current turn.
	GameState->Turns[GameState->Turns.Num() - 1]->Actions.Push(MoveAction);
}

//This function stores the necessary data to revert the death of the hamster in a kill action.
void ADHHamsterCharacter::AddKillAction()
{
	UDHKillAction* KillAction = NewObject<UDHKillAction>();

	KillAction->KilledActor = this;
	
	KillAction->OldPosition = GetActorLocation();
	
	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	//Add the action to the current turn.
	GameState->Turns[GameState->Turns.Num() - 1]->Actions.Push(KillAction);
}

//This function checks if a rat is perceived by the perception component. Meaning if there is a rat directly in front of
//the hamster.
bool ADHHamsterCharacter::IsRatInLoS()
{
	ADHHamsterController* HamsterController = Cast<ADHHamsterController>(GetController());

	checkf(HamsterController, TEXT("Hamster Controller not found"))

	UDHHamsterPerceptionComponent* PerceptionComponent = Cast<UDHHamsterPerceptionComponent>(HamsterController->GetPerceptionComponent());

	checkf(PerceptionComponent, TEXT("Perception Component not found"))

	//We sensed an actor before.
	if (LastSensedActor)
	{
		auto ActorInfo = PerceptionComponent->GetActorInfo(*LastSensedActor);
		
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(TArray<AActor*>{CurrentTile, this});

		QueryParams.AddIgnoredActors(Tiles);
		
		GetWorld()->LineTraceSingleByChannel(
				HitResult,
				GetActorLocation(),
				LastSensedActor->GetActorLocation(),
				ECC_GameTraceChannel1,
				QueryParams
		);

		if (HitResult.GetActor()) UE_LOG(LogTemp, Warning, TEXT("LoS Hit: %s"), *HitResult.GetActor()->GetName())
		
		//If the stimuli is still active when calling this function and it sensed a rat that wasn't moving, then the hamster can see it.
		CanSeeEnemy = HitResult.GetActor() && HitResult.GetActor()->IsA(ADHRat::StaticClass()) && ActorInfo->LastSensedStimuli[0].IsActive() && HitResult.GetActor()->GetVelocity() == FVector::ZeroVector;
	}

	return CanSeeEnemy;
}

//This function stops the movement of the hamster by resetting its blackboard values.
void ADHHamsterCharacter::StopMovement()
{
	ADHHamsterController* HamsterController = Cast<ADHHamsterController>(GetController());

	HamsterController->GetBlackboardComponent()->SetValueAsVector(TEXT("Destination"), FAISystem::InvalidLocation);

	HamsterController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanMove"), false);
}

void ADHHamsterCharacter::OnCloudNotTriggered_Implementation()
{
	
}

void ADHHamsterCharacter::UpdatePickaxeMaterial_Implementation(bool IsHamsterite)
{
	
}

void ADHHamsterCharacter::ResetEnemyVision()
{
	CanSeeEnemy = false;
}

void ADHHamsterCharacter::ResetLoS()
{
	IsRatInLoS();
}

//This function empties all queued movement.
void ADHHamsterCharacter::ResetQueuedMovement(AActor* Actor)
{
	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	GameState->IsDestroyingADestructible = false;
	
	QueuedMovement.Empty();
}

void ADHHamsterCharacter::WalkedIntoTrap_Implementation()
{
	
}

ADHHamsterCharacter::ADHHamsterCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	//Setting up components.
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComponent);

	Center = CreateDefaultSubobject<UBoxComponent>(TEXT("Center"));
	Center->SetupAttachment(RootComponent);

	PickaxeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickaxeMesh"));
	PickaxeMesh->SetupAttachment(GetMesh(), FName(TEXT("BackPack")));

}

void ADHHamsterCharacter::BeginPlay()
{
	Super::BeginPlay();

	CheckPossibleTiles();

	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	checkf(GameState, TEXT("Cannot find GameState"))

	//Binding events.
	GameState->OnTurnEnded.AddDynamic(this, &ADHHamsterCharacter::CheckPossibleTiles);

	GameState->OnMovementEnded.AddDynamic(this, &ADHHamsterCharacter::ResetEnemyVision);

	GameState->OnRevertEnded.AddDynamic(this, &ADHHamsterCharacter::ResetLoS);

	GameState->OnHamsterDied.AddDynamic(this, &ADHHamsterCharacter::ResetQueuedMovement);

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ADHHamsterCharacter::OnBeginOverlap);

	OnEnemyVisionUpdated.AddDynamic(this, &ADHHamsterCharacter::ResetLoS);
	
	//Fetching required actors.
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHFloorTile::StaticClass(), Tiles);
	
}

void ADHHamsterCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto* Pickup = Cast<IDHPickupInterface>(OtherActor);
	//Hamster overlapped with a pickable object.
	if (Pickup)
	{
		Pickup->TryPickup();
	}
}

void ADHHamsterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
