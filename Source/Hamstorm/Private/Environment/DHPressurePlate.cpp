// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/DHPressurePlate.h"

#include "Components/BoxComponent.h"
#include "Enemies/DHRat.h"
#include "Environment/DHTrap.h"
#include "Game/DHGameState.h"
#include "Game/DHTurn.h"
#include "Game/Actions/DHTrapAction.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Kismet/GameplayStatics.h"

ADHPressurePlate::ADHPressurePlate()
{
	PrimaryActorTick.bCanEverTick = false;

	//Setting up components.
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	
	RootComponent = StaticMeshComponent;

	Plate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plate"));

	Plate->SetupAttachment(RootComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));

	BoxComponent->SetupAttachment(RootComponent);

}

//This function stores the necessary data to revert the trap activation in a trap action.
void ADHPressurePlate::AddTrapAction()
{
	UDHTrapAction* TrapAction = NewObject<UDHTrapAction>();

	TrapAction->PressurePlate = this;

	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	//Add the action to the current turn.
	GameState->Turns[GameState->Turns.Num() - 1]->Actions.Push(TrapAction);
}

void ADHPressurePlate::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ADHPressurePlate::OnBeginOverlap);
}

void ADHPressurePlate::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//The overlapping actor isn't a rodent.
	if (!OtherActor->IsA(ADHRat::StaticClass()) && !OtherActor->IsA(ADHHamsterCharacter::StaticClass()))
		return;

	//Trap already activated.
	if (WasActivated) return;

	//No corresponding trap set. It is set in the editor.
	if (!Trap)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No corresponding trap"));
		return;
	}

	//Activate the trap otherwise.
	Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()))->IsTileTriggered = true;
	
	Trap->ActivateTrap();

	AddTrapAction();

	WasActivated = true;
}

