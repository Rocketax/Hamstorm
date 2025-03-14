// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Actions/DHTileHitAction.h"

#include "Environment/DHFloorTile.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DHPlayerController.h"

//This function restores the state of the tile to what it was before being weakened.
void UDHTileHitAction::RevertAction()
{
	HitTile->HasBeenHit = false;

	HitTile->OnBrokenTileReverted.Broadcast();

	ADHHamsterCharacter* HamsterCharacter = Cast<ADHHamsterCharacter>(UGameplayStatics::GetActorOfClass(HitTile->GetWorld(), ADHHamsterCharacter::StaticClass()));

	HamsterCharacter->PickaxeMesh->SetVisibility(true);

	ADHPlayerController* PlayerController = Cast<ADHPlayerController>(UGameplayStatics::GetPlayerController(HitTile->GetWorld(), 0));

	PlayerController->HasPickaxe = true;
}
