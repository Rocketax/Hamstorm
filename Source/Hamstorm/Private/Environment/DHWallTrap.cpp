// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/DHWallTrap.h"

#include "Components/ArrowComponent.h"
#include "Enemies/DHRat.h"
#include "Environment/DHPickaxe.h"
#include "Game/DHGameState.h"
#include "Game/DHTurn.h"
#include "Game/Actions/DHTrapAction.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Kismet/GameplayStatics.h"

ADHWallTrap::ADHWallTrap()
{
	//Setting up components.
	RootComponent = StaticMeshComponent;

	Shooter = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shooter"));

	Shooter->SetupAttachment(RootComponent);
	
	Projectile = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile"));

	Projectile->SetupAttachment(Shooter);

	ShootingDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootingDirection"));

	ShootingDirection->SetupAttachment(RootComponent);
}

void ADHWallTrap::OnTrapTriggered_Implementation()
{
	
}

//This function fires the projectile of the trap in a straight line.
void ADHWallTrap::ActivateTrap()
{
	Projectile->SetSimulatePhysics(true);
	
	Projectile->SetPhysicsLinearVelocity(ShootingDirection->GetForwardVector() * ProjectileSpeed);

	IsProjectileFired = true;

	OnWallTrapFiredUpdated.Broadcast();

	OnTrapTriggered();
}

//This function is used to rearm the trap when reverting its activation.
void ADHWallTrap::RearmTrap()
{
	Projectile->SetWorldLocation(InitialProjectileLocation);

	IsProjectileFired = false;

	OnWallTrapFiredUpdated.Broadcast();
}

//This is the overlap of the projectile.
void ADHWallTrap::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//The projectile might overlap sometimes when it isn't fired.
	if (!IsProjectileFired) return;

	//We ignore overlaps with the trap's mesh or pickaxes since they might be in the flight path of the projectile.
	if (OtherActor->IsA(ADHWallTrap::StaticClass()) || OtherActor->IsA(ADHPickaxe::StaticClass())) return;

	//The target hit was a rat.
	if (OtherActor->IsA(ADHRat::StaticClass()))
	{
		ADHRat* Rat = Cast<ADHRat>(OtherActor);

		Rat->IsDead = true;

		Rat->IsDying = true;

		Rat->StopMovement(false);

		Rat->OnRatDied.Broadcast();
	}
	//The target was the hamster.
	else if (OtherActor->IsA(ADHHamsterCharacter::StaticClass()))
	{
		ADHGameState* GameState = Cast<ADHGameState>(GetWorld()->GetGameState());
		
		GameState->OnHamsterDied.Broadcast(this);

		Cast<ADHHamsterCharacter>(OtherActor)->AddKillAction();
	}
	
	//Place the projectile under the map and stop its movement. It makes it easier to rearm the trap when reverting.
	Projectile->SetWorldLocation(FVector(0, 0, -10000));

	Projectile->SetSimulatePhysics(false);
	
	Projectile->SetPhysicsLinearVelocity(FVector(0, 0, 0));

	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));
	
	GameState->IsTileTriggered = false;

	GameState->CheckEndTurn();

	OnKillActor.Broadcast(OtherActor);
	
}

void ADHWallTrap::BeginPlay()
{
	Super::BeginPlay();

	Projectile->OnComponentBeginOverlap.AddDynamic(this, &ADHWallTrap::OnBeginOverlap);

	InitialProjectileLocation = Projectile->GetComponentLocation();
	
}
