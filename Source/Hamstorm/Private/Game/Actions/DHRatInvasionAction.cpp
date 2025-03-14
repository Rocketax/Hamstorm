// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Actions/DHRatInvasionAction.h"
#include "Kismet/GameplayStatics.h"

#include "Game/DHGameState.h"

//This function destroys the actors that were spawned in a rat invasion.
void UDHRatInvasionAction::RevertAction()
{
	UWorld* World = SpawnedActors[0]->GetWorld();
	
	for (AActor* SpawnedActor : SpawnedActors)
	{
		SpawnedActor->Destroy();
	}

	Cast<ADHGameState>(UGameplayStatics::GetGameState(World))->OnRatInvasionReverted.Broadcast();
}
