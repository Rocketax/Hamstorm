// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/DHCollectible.h"

#include "Components/BoxComponent.h"
#include "Game/DHGameState.h"
#include "Game/DHTurn.h"
#include "Game/Actions/DHPickupAction.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Kismet/GameplayStatics.h"

ADHCollectible::ADHCollectible()
{
	PrimaryActorTick.bCanEverTick = false;

	//Setting up components.
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	RootComponent = StaticMesh;
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetupAttachment(RootComponent);
}

void ADHCollectible::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//When hamster is overlapping with pickup object.
	if (OtherActor->IsA(ADHHamsterCharacter::StaticClass()))
	{
		TryPickup();
	}
}

void ADHCollectible::BeginPlay()
{
	Super::BeginPlay();
	
	AnimationPosition = GetActorLocation();
}

bool ADHCollectible::TryPickup()
{
	auto* GameState = UGameplayStatics::GetGameState(GetWorld());

	checkf(GameState, TEXT("No GameState"))

	ADHGameState* DHGameState = Cast<ADHGameState>(GameState);

	checkf(DHGameState, TEXT("No DHGameState"))

	//Overlaps can happen during revert. We want to avoid those.
	if (DHGameState->IsReverting) return false;

	DHGameState->IsCollectibleCollected = true;

	AddPickupAction();

	//Move object under the map after pickup. It is easier for the revert.
	AnimationPosition = FVector(0, 0, -1000);

	OnCollectibleCollected.Broadcast();
	
	return true;
}

//This function stores necessary data to revert picking up an object in a pickup action.
void ADHCollectible::AddPickupAction()
{
	UDHPickupAction* PickupAction = NewObject<UDHPickupAction>();

	PickupAction->PickupActor = this;

	PickupAction->Position = AnimationPosition;

	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	//Add the action to the current turn.
	GameState->Turns[GameState->Turns.Num() - 1]->Actions.Push(PickupAction);
}

