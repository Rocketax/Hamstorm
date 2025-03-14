// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/DHDestructiblePillar.h"

#include "Game/DHGameState.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DHPlayerController.h"

void ADHDestructiblePillar::BeginPlay()
{
	Super::BeginPlay();

	//Fetching required actors.
	PlayerController = Cast<ADHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	HamsterCharacter = Cast<ADHHamsterCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ADHHamsterCharacter::StaticClass()));
	GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));
}

void ADHDestructiblePillar::Destruct()
{
	AddDestroyAction();

	//Place the pillar under the map after destructing. It makes it easier for the revert.
	SetActorLocation(FVector(0, 0, -10000), false, nullptr, ETeleportType::ResetPhysics);
	
	PlayerController->OnWallDestroyed.Broadcast(this, HamsterCharacter);

	//Remove the pickaxe once used.
	PlayerController->HasHamsteritePickaxe = false;

	if (!PlayerController->HasPickaxe) PlayerController->OnAllPickaxesUsed();

	//Only add the pillar if it wasn't destroyed previously. Prevents glitch where destroying a pillar and reverting
	//multiple times completes the final level.
	if (!GameState->DestroyedPillars.Contains(this)) GameState->DestroyedPillars.Add(this);
	
	//Pillar needs to be associated with a zone. The zone name is set in the editor.
	if (ZoneName.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("ZoneName is not set in the pillar"));
	}

	GameState->OnPillarDestroyed.Broadcast(ZoneName);

	//We set a checkpoint after destroying a pillar so the reset brings the player back to that point.
	GameState->SetCheckpoint();

	ShouldBeDestroyed = false;

	UE_LOG(LogTemp, Warning, TEXT("HasPickaxe: %s, HasHamsteritePickaxe: %s"), PlayerController->HasPickaxe ? TEXT("true") : TEXT("false"), PlayerController->HasHamsteritePickaxe ? TEXT("true") : TEXT("false"))
}
