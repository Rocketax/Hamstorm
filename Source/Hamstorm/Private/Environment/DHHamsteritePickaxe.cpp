// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/DHHamsteritePickaxe.h"

#include "Game/DHGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DHPlayerController.h"

bool ADHHamsteritePickaxe::TryPickup()
{
	auto* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	//Overlaps can happen during revert. We ignore those.
	if (GameState->IsReverting) return false;
	
	auto* PlayerController = Cast<ADHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	checkf(PlayerController, TEXT("Cannot cast PlayerController"))

	//Player already has a hamsterite pickaxe
	if (PlayerController->HasHamsteritePickaxe) return false;

	PlayerController->HasHamsteritePickaxe = true;

	//First time player picks up a hamsterite pickaxe. Used for dialogue/tutorial.
	if (!PlayerController->HasPickedUpHamsteritePickaxe)
	{
		PlayerController->OnFirstHamsteritePickaxe();

		PlayerController->HasPickedUpHamsteritePickaxe = true;
	}
	
	AddPickupAction();

	//Place the pickaxe under the map. It makes it easier to revert its pickup.
	AnimationPosition = FVector(0, 0, -10000);

	OnPickup.Broadcast();
	
	return true;
}
