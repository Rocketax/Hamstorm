// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Actions/DHDestroyAction.h"

#include "Components/BoxComponent.h"
#include "Environment/DHDestructiblePillar.h"
#include "Environment/DHFloorTile.h"
#include "Game/DHGameState.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DHPlayerController.h"

//This function restores the state of the destroyed actor to what it was before being destroyed.
void UDHDestroyAction::RevertAction()
{
	ADHHamsterCharacter* HamsterCharacter = Cast<ADHHamsterCharacter>(UGameplayStatics::GetActorOfClass(DestroyedActor->GetWorld(), ADHHamsterCharacter::StaticClass()));

	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(DestroyedActor->GetWorld()));

	HamsterCharacter->PickaxeMesh->SetVisibility(true);

	ADHPlayerController* PlayerController = Cast<ADHPlayerController>(UGameplayStatics::GetPlayerController(DestroyedActor->GetWorld(), 0));

	//We handle the revert differently depending on the type of object that was destroyed.

	//A floor tile was destroyed.
	if (DestroyedActor->IsA(ADHFloorTile::StaticClass()))
	{
		ADHFloorTile* FloorTile = Cast<ADHFloorTile>(DestroyedActor);

		FloorTile->Destroyed = false;

		FloorTile->StaticMeshComponent->SetWorldLocation(OldPosition);
	}
	else DestroyedActor->SetActorLocation(OldPosition, false, nullptr, ETeleportType::ResetPhysics);

	//A pillar was destroyed.
	if (DestroyedActor->IsA(ADHDestructiblePillar::StaticClass()))
	{
		//The pillar was listed in the destroyed pillars array of the gamestate. We need to remove it to prevent the player
		//from reverting multiple times to complete the final level by destroying one pillar over and over.
		if (!GameState->DestroyedPillars.IsEmpty() && GameState->DestroyedPillars.Contains(DestroyedActor))
		{
			GameState->DestroyedPillars.Remove(DestroyedActor);
		}
		
		PlayerController->HasHamsteritePickaxe = true;
		
		HamsterCharacter->UpdatePickaxeMaterial(true);
	}
	else
	{
		PlayerController->HasPickaxe = true;

		HamsterCharacter->UpdatePickaxeMaterial(false);
	}

	//A wall was destroyed.
	if (DestroyedActor->IsA(ADHDestructibleWall::StaticClass()))
	{
		ADHDestructibleWall* Wall = Cast<ADHDestructibleWall>(DestroyedActor);
		
		Wall->OnWallDestroyedReverted.Broadcast();

		Wall->Destroyed = false;
	}
}
