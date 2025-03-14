// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DHGameState.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/DHDasherRat.h"
#include "Enemies/DHDasherRatController.h"
#include "Enemies/DHMovingRat.h"
#include "Enemies/DHMovingRatController.h"
#include "Game/DHTurn.h"
#include "Game/Actions/DHRatInvasionAction.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DHPlayerController.h"

void ADHGameState::BeginPlay()
{
	Super::BeginPlay();

	//Binding events
	OnMovementEnded.AddDynamic(this, &ADHGameState::TryEndTurn);

	OnRatMovementEnded.AddDynamic(this, &ADHGameState::UpdateNbRatsMoved);

	OnHamsterDeathFinished.AddDynamic(this, &ADHGameState::ResetRevertVariables);

	OnRatInvasionReverted.AddDynamic(this, &ADHGameState::ResetRatArrays);

	//Setting variables
	NbRatsKilled = 0;

	NbTurns = 0;

	IsTutorialCompleted = false;

	Turns.Push(NewObject<UDHTurn>());

	//Fetching required actors.	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHMovingRat::StaticClass(), MovingRats);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHRat::StaticClass(), Rats);
	HamsterCharacter = Cast<ADHHamsterCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ADHHamsterCharacter::StaticClass()));
	PlayerController = Cast<ADHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void ADHGameState::TryEndTurn()
{
	//Enemies are still attacking or hamster died. We don't want to end the turn yet.
	if (AreEnemiesAttacking() || HamsterCharacter->IsDead) return;

	HasAlreadyAddedTurn = false;

	CanRevertTurn = false;
	
	NbRatsMoved = 0;

	NbMovingRats = MovingRats.Num();

	//Adjusting the count of rats that need to move to end the turn.
	for (auto Rat : MovingRats)
	{
		if (Cast<ADHRat>(Rat)->IsDead)
		{
			NbMovingRats--;
		}
	}

	//We enable the movement of the rats after the hamster has moved.
	for (auto Rat : MovingRats)
	{
		//We skip dead rats.
		if (Cast<ADHRat>(Rat)->IsDead) continue;
		
		const auto MovingRat = Rat->IsA(ADHMovingRat::StaticClass()) ?
			Cast<ADHMovingRat>(Rat):
			Cast<ADHDasherRat>(Rat);
            	
        auto RatController = Rat->IsA(ADHMovingRat::StaticClass()) ?
        	Cast<ADHMovingRatController>(MovingRat->GetController()):
			Cast<ADHDasherRatController>(MovingRat->GetController());

        RatController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanMove"), true);
		
        RatController->SetDestination();
	}
	
	//Checks if the turn has ended (when all rats have moved).
	CheckEndTurn();
}

void ADHGameState::RevertTurn()
{
	//Depending on the player's actions, we might want to disable the revert.
	if (!CanRevertTurn || HamsterCharacter->IsMoving || IsDestroyingADestructible || PlayerController->IsInDialog || PlayerController->IsGamePaused) return;
	//There is no turn to revert.
	if (Turns.Num() == 0) return;
	//Only one turn has been set and it has no actions.
	if (Turns.Num() < 2 && Turns[Turns.Num() - 1]->Actions.Num() == 0) return;

	IsReverting = true;

	//There is no action in the current turn, so we revert the previous one.
	if (Turns[Turns.Num() - 1]->Actions.Num() == 0)
	{
		Turns[Turns.Num() - 2]->Revert();

		Turns.RemoveAt(Turns.Num() - 2);
	}
	//We can revert the current turn.
	else
	{
		Turns[Turns.Num() - 1]->Revert();
	}

	//Calling necessary functions to go back to the last turn.
	PlayerController->ResetDestructible();
	
	HamsterCharacter->CheckPossibleTiles();

	ResetAttackingRats();

	//Resetting variables
	IsReverting = false;
	HasReverted = true;
	IsHamstormDoneMoving = false;
	NbTurns++;

	OnRevertEnded.Broadcast();
	OnTurnEnded.Broadcast();
}

//This function checks and handles the end of a turn.
void ADHGameState::CheckEndTurn()
{
	//Not all rats are done moving or some actions need to be completed before ending the turn.
	if (NbRatsMoved < NbMovingRats || AreEnemiesAttacking() || AreEnemiesDying() || IsTileTriggered) return;

	IsHamstormDoneMoving = false;

	//Normal scenario. The player only clicked one tile further and there was at least one action in the turn.
	//We consider only one turn if the player clicked multiples tiles ahead to reward the ability to predict movements.
	if ((!IsQueuedMovement || !HamsterCharacter->QueuedMovement.Peek()) && !Turns[Turns.Num() - 1]->Actions.IsEmpty() &&
		!HasAlreadyAddedTurn)
	{
		NbTurns++;
		HasAlreadyAddedTurn = true;
		Turns.Push(NewObject<UDHTurn>());
	}

	IsQueuedMovement = false;

	if (!HamsterCharacter->IsDead)
		CanRevertTurn = true;

	ResetAttackingRats();

	OnTurnEnded.Broadcast();

	NbRatsMoved = 0;
}

void ADHGameState::UpdateNbRatsMoved()
{
	NbRatsMoved++;
	
	CheckEndTurn();
}

//This function stops all rats from moving and attacking. Making sure no one moves when the turn ended.
void ADHGameState::ResetAttackingRats()
{
	for (auto Rat : Rats)
	{
		ADHRatController* RatController = Cast<ADHRatController>(Cast<ADHRat>(Rat)->GetController());
		if (RatController->GetBlackboardComponent()->GetValueAsVector(TEXT("Destination")) == FAISystem::InvalidLocation) Cast<ADHRat>(Rat)->IsAttacking = false;
	}
}

//This function checks if there are enemies still attacking.
bool ADHGameState::AreEnemiesAttacking()
{
	bool IsAnyKilling = false;

	for (auto Rat : Rats)
	{
		if (Cast<ADHRat>(Rat)->IsAttacking) IsAnyKilling = true;
	}

	return IsAnyKilling;
}

//This function checks if there are enemies still dying.
bool ADHGameState::AreEnemiesDying()
{
	bool AreAnyRatsDying = false;

	for (auto Rat : Rats)
	{
		if (Cast<ADHRat>(Rat)->IsDying) AreAnyRatsDying = true;
	}
	
	return AreAnyRatsDying;
}

void ADHGameState::OnFirstTimeEnteringFinalZone_Implementation()
{
	
}

void ADHGameState::ResetRevertVariables()
{
	CanRevertTurn = true;

	HamsterCharacter->IsMoving = false;
}

//This function empties the turn array making it impossible to revert previous turns. Once you hit a checkpoint, you can
//only revert from that specific checkpoint.
void ADHGameState::SetCheckpoint()
{
	Turns.Empty();

	NbTurnsAtCheckpoint = NbTurns;
	
	UDHTurn* NewTurn = NewObject<UDHTurn>();
	
	Turns.Add(NewTurn);
}

//This function reverts all the turns after the checkpoint.
void ADHGameState::RestartFromCheckpoint()
{
	//Exit conditions that prevent turn revert.
	if (!CanRevertTurn || HamsterCharacter->IsMoving) return;
	if (Turns.Num() == 0) return;
	if (Turns.Num() < 2 && Turns[Turns.Num() - 1]->Actions.Num() == 0) return;
	
	while (Turns.Num() != 1)
	{
		RevertTurn();
	}

	RevertTurn();

	//Keep the number of turns in memory to display in the HUD.
	NbTurns = NbTurnsAtCheckpoint;

	SetCheckpoint();

	OnTurnEnded.Broadcast();
}

//This function stores the necessary data to revert a rat invasion in a rat invasion action.
void ADHGameState::AddRatInvasionAction(TArray<AActor*> InvadingRats)
{
	UDHRatInvasionAction* RatInvasionAction = NewObject<UDHRatInvasionAction>();

	RatInvasionAction->SpawnedActors = InvadingRats;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHMovingRat::StaticClass(), MovingRats);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHRat::StaticClass(), Rats);

	//Add the action to the current turn.
	Turns[Turns.Num() - 1]->Actions.Push(RatInvasionAction);
}

void ADHGameState::ResetRatArrays()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHMovingRat::StaticClass(), MovingRats);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADHRat::StaticClass(), Rats);
}
