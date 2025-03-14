// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/DHRat.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/DHDasherRat.h"
#include "Enemies/DHDasherRatController.h"
#include "Enemies/DHMovingRat.h"
#include "Enemies/DHMovingRatController.h"
#include "Enemies/DHRatController.h"
#include "Environment/DHDestructibleWall.h"
#include "Environment/DHPickaxe.h"
#include "Game/DHGameState.h"
#include "Game/DHTurn.h"
#include "Game/Actions/DHKillAction.h"
#include "Game/Actions/DHMoveAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Hamster/DHHamsterController.h"
#include "Hamster/Components/DHHamsterPerceptionStimuli.h"
#include "Kismet/GameplayStatics.h"

void ADHRat::OnDyingBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                               int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()))->IsReverting) return;
	
	//If there is an overlap between the rat's dying box (while it isn't attacking) and the hamster, we trigger the rat's death.
	if (!IsAttacking && OtherActor->IsA(ADHHamsterCharacter::StaticClass()))
	{
		auto* HamsterCharacter = Cast<ADHHamsterCharacter>(OtherActor);
			
		if (!HamsterCharacter->IsDead)
		{
			if (IsA(ADHMovingRat::StaticClass())) Cast<ADHMovingRat>(this)->RevertTransform = GetTransform();

			if (!HamsterCharacter->IsAttacking)
			{
				//Trigger the battling animation for the hamster.
				HamsterCharacter->OnCloudNotTriggered();
			}
				
			Die(false);
		} 
	}
}

void ADHRat::OnKillingBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()))->IsReverting) return;
	
	if (OtherActor->IsA(ADHHamsterCharacter::StaticClass()))
	{
		//Hamster overlapped with killing box. Now, we need to verify the rat is looking at it.
		FHitResult HitResult;
		
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActors(Pickaxes);
		QueryParams.AddIgnoredActors(Rats);
		
		GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 400, ECC_Visibility, QueryParams);
		
		//The rat isn't looking at the hamster.
		if (HitResult.GetActor() && !HitResult.GetActor()->IsA(ADHHamsterCharacter::StaticClass())) return;
		
		IsAttacking = true;
		
		if (IsA(ADHMovingRat::StaticClass()))
		{
			//Store transform before moving.
			Cast<ADHMovingRat>(this)->RevertTransform = GetActorTransform();
		}
		
		ADHRatController* RatController = Cast<ADHRatController>(GetController());

		if (RatController)
		{
			//Set the destination to be at the hamster's location and keep the rat's height.
			auto Destination = OverlappedComp->GetComponentLocation();
			Destination.Z = GetActorLocation().Z;

			if (!IsMoving)
			{
				//Move towards the hamster.
				AddMoveAction();
			}

			StopMovement();
			
			ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

			checkf(GameState, TEXT("GameState not found"))

			auto HamsterCharacter = Cast<ADHHamsterCharacter>(OtherActor);

			if (!HamsterCharacter->IsDead)
			{
				//Kill the hamster.
				HamsterCharacter->AddKillAction();
				
				GameState->OnHamsterDied.Broadcast(this);
			}
		}
	}
}

ADHRat::ADHRat()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Setting up components.
	DyingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DyingBox"));
	DyingBox->SetCollisionObjectType(ECC_GameTraceChannel2);
	DyingBox->SetupAttachment(GetRootComponent());

	Center = CreateDefaultSubobject<UBoxComponent>(TEXT("Center"));
	Center->SetupAttachment(GetRootComponent());
	
	KillingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("KillingBox"));
	KillingBox->SetCollisionObjectType(ECC_GameTraceChannel2);
	KillingBox->SetupAttachment(GetRootComponent());

	StimuliComponent = CreateDefaultSubobject<UDHHamsterPerceptionStimuli>(TEXT("StimuliComponent"));

	TeamId = FGenericTeamId(0);
}

// Called when the game starts or when spawned.
void ADHRat::BeginPlay()
{
	Super::BeginPlay();
	
	this->DyingBox->OnComponentBeginOverlap.AddDynamic(this, &ADHRat::OnDyingBoxOverlap);

	this->KillingBox->OnComponentBeginOverlap.AddDynamic(this, &ADHRat::OnKillingBoxOverlap);

	//Fetching required actors.
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHPickaxe::StaticClass(), Pickaxes);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHRat::StaticClass(), Rats);
}

void ADHRat::Die(bool ShouldStopMovement)
{
	if (HasAlreadyDied || Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()))->IsReverting) return;
	
	//Set variables for the turn to end properly.
	IsDead = true;

	IsAttacking = false;

	HasAlreadyDied = true;

	AddKillAction();

	if (ShouldStopMovement) StopMovement();
	
	//Placing the actor under the map so we don't need to respawn it. It makes it easier when reverting.
	SetActorLocation(FVector(0 ,0,-1000), false, nullptr, ETeleportType::ResetPhysics);
     
	ADHGameState* DHGameState = Cast<ADHGameState>(GetWorld()->GetGameState());
     
	DHGameState->NbRatsKilled++;
	if (IsDying)
	{
		IsDying = false;
		DHGameState->CheckEndTurn();
	}
}

void ADHRat::StopMovement(bool ShouldEndTurn)
{
	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));
	
	ADHRatController* RatController = Cast<ADHRatController>(GetController());

	GetCharacterMovement()->Velocity = FVector(0, 0, 0);

    //Reset blackboard variables to prevent movement.
	RatController->GetBlackboardComponent()->SetValueAsVector(TEXT("Destination"), FAISystem::InvalidLocation);
        
	RatController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanMove"), false);
	
	if (ShouldEndTurn) GameState->OnRatMovementEnded.Broadcast();
}

//This function stores the necessary data to revert a kill in a kill action.
void ADHRat::AddKillAction()
{
	UDHKillAction* KillAction = NewObject<UDHKillAction>();

	KillAction->KilledActor = this;

	//Storing previous location according to the rat type.
	if(KillAction->KilledActor->IsA(ADHMovingRat::StaticClass()))
	{
		ADHMovingRat* MovingRat = Cast<ADHMovingRat>(KillAction->KilledActor);
		
		KillAction->OldPosition = MovingRat->RevertTransform.GetLocation();
	}
	else
	{
		KillAction->OldPosition = GetActorLocation();
	}
	
	//Add the action in the current turn.
	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	GameState->Turns[GameState->Turns.Num() - 1]->Actions.Push(KillAction);
}

//This function stores the necessary data to revert a movement in a move action.
void ADHRat::AddMoveAction()
{
	UDHMoveAction* MoveAction = NewObject<UDHMoveAction>();

	MoveAction->MovedActor = this;

	MoveAction->RevertTransform = GetActorTransform();

	MoveAction->IsAttacking = IsAttacking;

	//Add the action in the current turn.
	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	GameState->Turns[GameState->Turns.Num() - 1]->Actions.Push(MoveAction);
}

FGenericTeamId ADHRat::GetGenericTeamId() const
{
	return TeamId;
}
