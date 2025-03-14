// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/DHFinalZoneTrigger.h"

#include "Components/BoxComponent.h"
#include "Game/DHGameState.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Kismet/GameplayStatics.h"

ADHFinalZoneTrigger::ADHFinalZoneTrigger()
{

	PrimaryActorTick.bCanEverTick = false;

	//Setting up overlap component.
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));

}

void ADHFinalZoneTrigger::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ADHFinalZoneTrigger::OnBeginOverlap);
	
}

void ADHFinalZoneTrigger::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Hamster enters final zone trigger
	if (OtherActor->IsA(ADHHamsterCharacter::StaticClass()))
	{
		ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

		//Hamster enters a different zone from the current one
		if (GameState->CurrentZone != Zone)
		{
			if (GameState->IsFirstTimeEnteringFinalZone)
			{
				GameState->IsFirstTimeEnteringFinalZone = false;

				GameState->OnFirstTimeEnteringFinalZone();
			}
			//Set the current zone and checkpoint. A zone is handled as a mini level.
			GameState->CurrentZone = Zone;

			GameState->SetCheckpoint();

			GameState->OnZoneChanged.Broadcast();
		}
	}
}
