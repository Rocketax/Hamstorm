// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/DHPickaxe.h"

#include "Components/BoxComponent.h"
#include "Environment/DHHamsteritePickaxe.h"
#include "Game/DHGameState.h"
#include "Game/DHTurn.h"
#include "Game/Actions/DHPickupAction.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DHPlayerController.h"

ADHPickaxe::ADHPickaxe()
{
	PrimaryActorTick.bCanEverTick = false;

	//Setting up components.
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	RootComponent = SceneComponent;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	StaticMeshComponent->SetupAttachment(SceneComponent);
}

void ADHPickaxe::BeginPlay()
{
	Super::BeginPlay();

	AnimationPosition = GetActorLocation();
}

bool ADHPickaxe::TryPickup()
{
	auto* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	//Overlaps can happen during revert. We ignore those.
	if (GameState->IsReverting) return false;
	
	auto* PlayerController = Cast<ADHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	checkf(PlayerController, TEXT("Cannot cast PlayerController"))

	//Player already has a pickaxe
	if (PlayerController->HasPickaxe) return false;

	PlayerController->HasPickaxe = true;
	
	AddPickupAction();

	//Place the pickaxe under the map. It makes it easier to revert its pickup.
	AnimationPosition = FVector(0, 0, -10000);

	OnPickup.Broadcast();

	IsPickedUp = true;
	
	return true;
}

//This function stores the necessary data to revert a pickup in a pickup action.
void ADHPickaxe::AddPickupAction()
{
	UDHPickupAction* PickupAction = NewObject<UDHPickupAction>();

	PickupAction->PickupActor = this;

	PickupAction->Position = AnimationPosition;

	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	//Add the action to the current turn.
	GameState->Turns[GameState->Turns.Num() - 1]->Actions.Push(PickupAction);
}
