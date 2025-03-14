// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Actions/DHPickupAction.h"

#include "Environment/DHCollectible.h"
#include "Environment/DHHamsteritePickaxe.h"
#include "Environment/DHPickaxe.h"
#include "Game/DHGameMode.h"
#include "Game/DHGameState.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DHPlayerController.h"

//This function restores the state of an item that was picked up to what it was before the pickup.
void UDHPickupAction::RevertAction()
{
	PickupActor->SetActorLocation(Position, false, nullptr, ETeleportType::ResetPhysics);

	ADHHamsterCharacter* HamsterCharacter = Cast<ADHHamsterCharacter>(UGameplayStatics::GetActorOfClass(PickupActor->GetWorld(), ADHHamsterCharacter::StaticClass()));

	checkf(HamsterCharacter, TEXT("Hamster is null"))
	
	//A pickaxe was picked up.
	if (PickupActor->IsA(ADHPickaxe::StaticClass()))
	{
		ADHPickaxe* Pickaxe = Cast<ADHPickaxe>(PickupActor);
		
		Pickaxe->AnimationPosition = Position;

		Pickaxe->IsPickedUp = false;
		
		HamsterCharacter->PickaxeMesh->SetVisibility(false);
	}

	//A collectible was picked up.
	if (PickupActor->IsA(ADHCollectible::StaticClass()))
	{
		auto* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(PickupActor->GetWorld()));
		
		Cast<ADHCollectible>(PickupActor)->AnimationPosition = Position;

		GameState->IsCollectibleCollected = false;
	}

	ADHPlayerController* PlayerController = Cast<ADHPlayerController>(UGameplayStatics::GetPlayerController(PickupActor->GetWorld(), 0));

	//Handling the different pickaxes.
	if (PickupActor->IsA(ADHHamsteritePickaxe::StaticClass()))
	{
		PlayerController->HasHamsteritePickaxe = false;

		HamsterCharacter->UpdatePickaxeMaterial(true);
	}
	else if (PickupActor->IsA(ADHPickaxe::StaticClass()))
	{
		PlayerController->HasPickaxe = false;

		HamsterCharacter->UpdatePickaxeMaterial(false);
	}
}
