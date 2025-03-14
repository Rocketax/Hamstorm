// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/DHFloorTile.h"

#include "Components/BoxComponent.h"
#include "Enemies/DHDasherRat.h"
#include "Enemies/DHRat.h"
#include "Game/DHGameState.h"
#include "Game/DHTurn.h"
#include "Game/Actions/DHDestroyAction.h"
#include "Game/Actions/DHTileHitAction.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DHPlayerController.h"

ADHFloorTile::ADHFloorTile()
{
	PrimaryActorTick.bCanEverTick = false;

	//Setting up components.
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	
	BoxComponent->SetupAttachment(RootComponent);
	
} 

void ADHFloorTile::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ADHFloorTile::OnBeginOverlap);

	//FloorType 0 is the floor in between tiles. The only time we consider it is when it is destructible.
	if (FloorType != 0 || Destructible)
	{
		//Adjust hitboxes for raycast and overlaps.
		BoxComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
		if (Destructible) BoxComponent->SetBoxExtent(FVector(10, 10, 75));
		else  BoxComponent->SetBoxExtent(FVector(40, 40, 75));
	}
	//We ignore this floor.
	else
	{
		//Avoid all collisions with actors and raycast.
		BoxComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		BoxComponent->SetBoxExtent(FVector(0, 0, 0));
	}

	//Fetching required actors.
	HamsterCharacter = Cast<ADHHamsterCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ADHHamsterCharacter::StaticClass()));
	GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));
}

//This event is used to display changes in real time when changing the floor type in the editor. It changes the mesh and orientation of it.
//It is mainly there to help level artists when creating levels in the editor.
void ADHFloorTile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (GetWorld())
	{
		//Changing mesh and rotation.
		const FString AssetPath = FString::Printf(TEXT("/Game/Meshes/Modulaires/Floors/SM_PlancherNavigable_0%d.SM_PlancherNavigable_0%d"),
										   FloorType, FloorType);
		UStaticMesh* FloorMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *AssetPath));

		StaticMeshComponent->SetStaticMesh(FloorMesh);
		
		SetActorRelativeRotation(FRotator(0, FloorRotationZ * 90, 0));
	}
}

void ADHFloorTile::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Hamster is on the tile.
	if (OtherActor->IsA(ADHHamsterCharacter::StaticClass()) && !Destructible)
	{
		HamsterCharacter->CurrentTile = this;
	}

	//The tile is destructible and has been weakened. It will be destroyed when a rodent walks on it.
	if ((OtherActor->IsA(ADHHamsterCharacter::StaticClass()) || OtherActor->IsA(ADHRat::StaticClass()))
		&& Destructible
		&& HasBeenHit)
	{
		//Hamter set the trap.
		if (OtherActor->IsA(ADHHamsterCharacter::StaticClass()))
		{
			HamsterCharacter->WalkedIntoTrap();
			
			HamsterCharacter->StopMovement();

			HamsterCharacter->ResetQueuedMovement(nullptr);

			HamsterCharacter->IsFalling = true;
		}
		
		//Dasher rat set the trap.
		if (OtherActor->IsA(ADHDasherRat::StaticClass()))
		{
			ADHDasherRat* Rat = Cast<ADHDasherRat>(OtherActor);
			
			Rat->StopMovement(false);

			Rat->IsFalling = true;
		}
		
		//Moving rat set the trap.
		if (OtherActor->IsA(ADHMovingRat::StaticClass()))
		{
			ADHMovingRat* Rat = Cast<ADHMovingRat>(OtherActor);
			
			Rat->StopMovement(false);

			Rat->IsFalling = true;
		}

		//Rat set the trap
		if (OtherActor->IsA(ADHRat::StaticClass()))
		{
			ADHRat* Rat = Cast<ADHRat>(OtherActor);
			
			Rat->StopMovement(false);

			Rat->IsFalling = true;
		}
		
		//If the trap was already destroyed, we stop there.
		if (Destroyed) return;
		
		Destroyed = true;

		OnBrokenTileDestroyed.Broadcast();

		AddDestroyAction();

		BoxComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		//We place the floor under the map after destruction. It makes it easier to revert its destruction.
		StaticMeshComponent->SetWorldLocation(FVector(0, 0, -3000), false, nullptr, ETeleportType::ResetPhysics);	
	}
}

void ADHFloorTile::SetShouldBeDestroyed(bool newValue)
{
	this->ShouldBeDestroyed = newValue;
}

//This function is misnamed, it just weakens the destructible floor tile.
void ADHFloorTile::Destruct()
{
	//The floor is destructible and not weakened.
	if (Destructible && !HasBeenHit)
	{
		HasBeenHit = true;

		AddHitAction();

		auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

		auto DHPlayerController = Cast<ADHPlayerController>(PlayerController);

		checkf(DHPlayerController, TEXT("No Player controller"))
	
		DHPlayerController->OnWallDestroyed.Broadcast(this, HamsterCharacter);

		//Consume the pickaxe once used.
		DHPlayerController->HasPickaxe = false;

		if (!DHPlayerController->HasHamsteritePickaxe) DHPlayerController->OnAllPickaxesUsed();

		ShouldBeDestroyed = false;
	}
}

//This function looks for an adjacent tile that is accessible by the hamster. It returns it if it finds one.
ADHFloorTile* ADHFloorTile::CheckValidAdjacentTile()
{
	FHitResult HitResult;

	ADHFloorTile* ValidTile = nullptr;

	//Ignore actors that would block the raycast.
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(HamsterCharacter);

	//Raycast in all four directions to check for adjacent tiles.
	for (int i = 0; i < 4; i++)
	{
		FVector Direction = GetActorForwardVector().RotateAngleAxis(90 * i, FVector(0, 0, 1));

		if (GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), GetActorLocation() + Direction * 400,
												 ECC_GameTraceChannel1, QueryParams))
		{
			//We hit a tile and it is accessible by the hamster.
			if (HitResult.GetActor()->IsA(ADHFloorTile::StaticClass())
				&& HamsterCharacter->PossibleTiles.Contains(HitResult.GetActor()) || HamsterCharacter->CurrentTile == HitResult.GetActor())
			{
				//We don't have a valid tile or the hamster is closer to the current tile than the previous one found.
				//This condition is used if the hamster could walk over the trap to the next adjacent tile. In that situation,
				//we choose the closest tile to the hamster.
				if (!ValidTile ||
					FVector::Dist(HitResult.GetActor()->GetActorLocation(), HamsterCharacter->GetActorLocation())
					< FVector::Dist(ValidTile->GetActorLocation(), HamsterCharacter->GetActorLocation()))
						
				ValidTile = Cast<ADHFloorTile>(HitResult.GetActor());
			}
		}
	}

	return ValidTile;
}

//This function stores the necessary data to revert the destruction of a floor tile in a destroy action.
void ADHFloorTile::AddDestroyAction()
{
	UDHDestroyAction* DestroyAction = NewObject<UDHDestroyAction>();

	DestroyAction->DestroyedActor = this;

	DestroyAction->OldPosition = StaticMeshComponent->GetComponentLocation();

	//Add the action to the current turn.
	GameState->Turns[GameState->Turns.Num() - 1]->Actions.Push(DestroyAction);
}

//This function stores the necessary data to revert the weakening of a floor tile in a hit action.
void ADHFloorTile::AddHitAction()
{
	UDHTileHitAction* TileHitAction = NewObject<UDHTileHitAction>();

	TileHitAction->HitTile = this;

	//Add the action to the current turn.
	GameState->Turns[GameState->Turns.Num() - 1]->Actions.Push(TileHitAction);
}