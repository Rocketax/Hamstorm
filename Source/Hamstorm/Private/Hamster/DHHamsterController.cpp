// Fill out your copyright notice in the Description page of Project Settings.


#include "Hamster/DHHamsterController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/DHRat.h"
#include "Environment/DHFloorTile.h"
#include "Environment/DHPickaxe.h"
#include "Game/DHGameState.h"
#include "Game/DHTurn.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Hamster/Components/DHHamsterPerceptionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DHPlayerController.h"


ADHHamsterController::ADHHamsterController()
{
	PrimaryActorTick.bCanEverTick = true;

	//Setting up perception component.
	PerceptionComponent = CreateDefaultSubobject<UDHHamsterPerceptionComponent>(TEXT("PerceptionComponent"));
}

void ADHHamsterController::SetDestination(AActor* ClickedActor)
{
	checkf(Blackboard, TEXT("No blackboard component"))
	
	ADHHamsterCharacter* HamsterCharacter = Cast<ADHHamsterCharacter>(GetPawn());

	checkf(HamsterCharacter, TEXT("Hamster Character not found"))

	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));
	
	//The player has clicked an actor.
	if (ClickedActor)
	{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(TArray<AActor*>{HamsterCharacter->CurrentTile, HamsterCharacter});

		bool HasHit = false;
    
        AActor* ChosenActor = ClickedActor;

		do
		{
			//We raycast to check if there is an obstacle between the hamster and the clicked actor.
			HasHit = GetWorld()->LineTraceSingleByChannel(
            	HitResult,
            	HamsterCharacter->GetActorLocation(),
            	ClickedActor->GetActorLocation(),
            	ECC_GameTraceChannel1,
            	QueryParams
            );
    
			//There is an obstacle between the hamster and the clicked actor.
            if (HitResult.GetActor() && HitResult.GetActor() != ClickedActor)
            {
				//The obstacle is a tile. We can go over it.
            	if (HitResult.GetActor()->IsA(ADHFloorTile::StaticClass()))
            	{
            		auto Tile = Cast<ADHFloorTile>(HitResult.GetActor());
					
					//We check if the tile is destructible since if it is, it has a hitbox and can be hit during raycasts.
					//The destructible tiles are the same as the tiles that link accessible tiles. Hamstorm can't stay on them.
					//If it is an accessible tile, we add this tile to the queued movement since it can be used to reach the clicked actor.
            		if (!Tile->Destructible)
            		{
            			ChosenActor = HitResult.GetActor();
            			HamsterCharacter->QueuedMovement.Enqueue(ClickedActor);
            			GameState->IsQueuedMovement = true;
            			HasHit = false;
            		}
					//It is a destructible tile, we want to ignore it for the next raycast.
            		else
            		{
            			QueryParams.AddIgnoredActor(Tile);
            		}
            	}
				//The obstacle isn't a tile. It could be a pickaxe or something the hamster could go over. That is why
				//we ingore this actor as well.
            	else
            	{
            		QueryParams.AddIgnoredActor(HitResult.GetActor());
            	}
            }
			//There was no obstacle between the hamster and the clicked actor.
            else
            {
            	ChosenActor = ClickedActor;
            	HasHit = false;
            }
            
		} while (HasHit);
		
        FVector Destination = ChosenActor->GetActorLocation();
		Destination.Z = HamsterCharacter->GetActorLocation().Z;

		QueryParams.ClearIgnoredActors();
		QueryParams.AddIgnoredActors(TArray<AActor*>{HamsterCharacter->CurrentTile, HamsterCharacter});
		QueryParams.AddIgnoredActors(Pickaxes);

		HasHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				HamsterCharacter->GetActorLocation(),
				Destination,
				ECC_WorldDynamic,
				QueryParams
		);

		//There is a rat in the way of the movement. We attack it.
		if (HitResult.GetActor() && HitResult.GetActor()->IsA(ADHRat::StaticClass()))
		{
			HamsterCharacter->OnAttackRat.Broadcast(HitResult.GetActor()->GetActorLocation());
		}

		if (HamsterCharacter->HasUsedPipe)
		{
			Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()))->Turns.Push(NewObject<UDHTurn>());
			
			HamsterCharacter->HasUsedPipe = false;
		}
		
		HamsterCharacter->AddMoveAction();

		HamsterCharacter->IsMoving = true;
	
		//The path is clear.
		if (!HitResult.GetActor() || (HitResult.GetActor() && !HitResult.GetActor()->IsA(ADHRat::StaticClass()))) GetBlackboardComponent()->SetValueAsVector(TEXT("Destination"), Destination);

		// If the movement is done next turn, we clear the possible tiles.
		if (!HamsterCharacter->QueuedMovement.Peek()) HamsterCharacter->PossibleTiles.Empty();
	}
	
}

//This function checks if the chosen tile is accessible.
void ADHHamsterController::ValidateTile(AActor* Destination)
{
	ADHHamsterCharacter* HamsterCharacter = Cast<ADHHamsterCharacter>(GetPawn());

	checkf(HamsterCharacter, TEXT("Hamster Character not found"))

	if(HamsterCharacter->PossibleTiles.Contains(Destination))
	{
		auto* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

		GameState->CanRevertTurn = false;
		GameState->IsHamstormDoneMoving = true;
		
		HamsterCharacter->IsTileChosen = true;
		HamsterCharacter->OnTileChosen.Broadcast(Destination, HamsterCharacter->IsAttacking);
	}
}

void ADHHamsterController::BeginPlay()
{
	Super::BeginPlay();

	//Binding events.
	auto* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	checkf(PlayerController, TEXT("No player controller found"))

	ADHPlayerController* DHPlayerController = Cast<ADHPlayerController>(PlayerController);

	DHPlayerController->OnTryCharacterMove.AddDynamic(this, &ADHHamsterController::ValidateTile);

	ADHHamsterCharacter* HamsterCharacter = Cast<ADHHamsterCharacter>(GetCharacter());

	checkf(HamsterCharacter, TEXT("Cannot find HamsterCharacter"))

	HamsterCharacter->OnCharacterMove.AddDynamic(this, &ADHHamsterController::SetDestination);


	checkf(BehaviorTree, TEXT("No behaviour tree"))

	RunBehaviorTree(BehaviorTree);

	//Fetching required actors.
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHPickaxe::StaticClass(), Pickaxes);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHFloorTile::StaticClass(), Tiles);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHRat::StaticClass(), Rats);
}
