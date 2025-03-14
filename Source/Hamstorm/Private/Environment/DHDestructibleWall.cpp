// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/DHDestructibleWall.h"

#include "Enemies/DHRat.h"
#include "Environment/DHDestructiblePillar.h"
#include "Environment/DHFloorTile.h"
#include "Game/DHGameState.h"
#include "Game/DHTurn.h"
#include "Game/Actions/DHDestroyAction.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DHPlayerController.h"

ADHDestructibleWall::ADHDestructibleWall()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

}

void ADHDestructibleWall::BeginPlay()
{
	Super::BeginPlay();
	
	//Fetching required actors.
	PlayerController = Cast<ADHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	HamsterCharacter = Cast<ADHHamsterCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ADHHamsterCharacter::StaticClass()));
	GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));
}

void ADHDestructibleWall::SetShouldBeDestroyed(bool newValue)
{
	ShouldBeDestroyed = newValue;
}

void ADHDestructibleWall::Destruct()
{
	AddDestroyAction();

	//Place the wall under the map after destruction. It makes it easier to revert the destruction.
	SetActorLocation(FVector(0, 0, -2000), false, nullptr, ETeleportType::ResetPhysics);

	Destroyed = true;
	
	PlayerController->OnWallDestroyed.Broadcast(this, HamsterCharacter);

	//Consume the pickaxe once used.
	PlayerController->HasPickaxe = false;

	if (!PlayerController->HasHamsteritePickaxe) PlayerController->OnAllPickaxesUsed();

	ShouldBeDestroyed = false;
}

//This function checks the adjacent tiles and verifies if one of them is accessible by the hamster.
//The hamster has to be able to go next to the destructible wall to destroy it. It returns a valid tile if there is one.
ADHFloorTile* ADHDestructibleWall::CheckValidAdjacentTile()
{
	FHitResult HitResult;

	ADHFloorTile* ValidTile = nullptr;

	//We ignore all actors that could block the check for the tiles.
	TArray<AActor*> CollidingActors;
	GetOverlappingActors(CollidingActors, ADHFloorTile::StaticClass());

	TArray<AActor*> Rats;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHRat::StaticClass(), Rats);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(HamsterCharacter);
	QueryParams.AddIgnoredActors(CollidingActors);
	QueryParams.AddIgnoredActors(Rats);

	FVector StartingPoint =  GetActorLocation();

	//Adjusting raycast height.
	if (IsA(ADHDestructiblePillar::StaticClass())) StartingPoint.Z += 200;
	else if (StartingPoint.Z < 0) StartingPoint.Z = 50;

	//Raycast in all 4 directions to check adjacent tiles.
	for (int i = 0; i < 4; i++)
	{
		FVector Direction = GetActorForwardVector().RotateAngleAxis(90 * i, FVector(0, 0, 1));

		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartingPoint, StartingPoint + Direction * 400,
		                                         ECC_GameTraceChannel1, QueryParams))
		{
			//We detected an adjacent tile and it is accessible by the hamster.
			if (HitResult.GetActor()->IsA(ADHFloorTile::StaticClass())
				&& (HamsterCharacter->PossibleTiles.Contains(HitResult.GetActor()) || HamsterCharacter->CurrentTile == HitResult.GetActor()))
			{
				return ValidTile = Cast<ADHFloorTile>(HitResult.GetActor()); 
			}
		}
	}

	return ValidTile;
	
}

//This function stores the necessary data to revert the destruction in a destroy action.
void ADHDestructibleWall::AddDestroyAction()
{
	UDHDestroyAction* DestroyAction = NewObject<UDHDestroyAction>();

	DestroyAction->DestroyedActor = this;

	DestroyAction->OldPosition = GetActorLocation();

	//Add the action to the current turn.
	GameState->Turns[GameState->Turns.Num() - 1]->Actions.Push(DestroyAction);
}
