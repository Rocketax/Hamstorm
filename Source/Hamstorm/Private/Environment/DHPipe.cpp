// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/DHPipe.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Enemies/DHRat.h"
#include "Game/DHGameState.h"
#include "Game/DHTurn.h"
#include "Game/Actions/DHTeleportAction.h"
#include "GameFramework/SpringArmComponent.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/DHPlayerController.h"
#include "Player/DHPlayerPawn.h"

ADHPipe::ADHPipe()
{
	PrimaryActorTick.bCanEverTick = false;

	//Setting up components.
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));

	ArrowComponent->SetupAttachment(StaticMeshComponent);

	TeleportingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("TeleportingPoint"));

	TeleportingPoint->SetupAttachment(StaticMeshComponent);
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	
	BoxComponent->SetupAttachment(StaticMeshComponent);

}

//This function is called when clicking a pipe
void ADHPipe::UsePipe()
{
	//No corresponding pipe was set as the other side. This is set in the editor.
	if (!OtherSide)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("" + GetName() + ": No corresponding pipe"));
		return;
	}
	//The player has clicked directly on the pipe when he/she wasn't on the adjacent tile. We will teleport them once the
	//hamster has reached the adjacent tile and the turn ends.
	if (OtherSide->AutoTeleport) return;

	//Hamster can die after clicking on the pipe. If that is the case we remove the dynamic for pipe use at the end of the turn.
	if (HamsterCharacter->IsDead)
	{
		GameState->OnTurnEnded.RemoveAll(this);
		return;
	}

	//If the player clicks further tiles and passes over the pipe, we don't want to use the pipe.
	if (!HamsterCharacter->QueuedMovement.IsEmpty()) return;

	OnStartTeleportation.Broadcast();

	HamsterCharacter->IsMoving = true;

	//We disable camera inputs since we move it when teleporting.
	PlayerController->CanMoveCamera = false;
}

//This function stores the necessary data to revert the teleportation in a teleport action.
void ADHPipe::AddTeleportAction(AActor* Hamster)
{
	UDHTeleportAction* TeleportAction = NewObject<UDHTeleportAction>();

	TeleportAction->TeleportedActor = Hamster;

	TeleportAction->InitialPipe = this;

	TeleportAction->OldTransform = Hamster->GetActorTransform();

	//Add the action to the current turn.
	GameState->Turns[GameState->Turns.Num() - 1]->Actions.Push(TeleportAction);
}

void ADHPipe::Teleport()
{
	AddTeleportAction(HamsterCharacter);

	ADHPlayerPawn* PlayerPawn = Cast<ADHPlayerPawn>(PlayerController->GetPawn());
	
	//We teleport the hamster to the other end of the pipe.
	HamsterCharacter->SetActorLocation(OtherSide->TeleportingPoint->GetComponentLocation(), false, nullptr, ETeleportType::ResetPhysics);
	
	//We rotate the hamster to the desired rotation of the output pipe.
 	FRotator PipeRotator = UKismetMathLibrary::FindLookAtRotation(HamsterCharacter->GetActorLocation(),
 		HamsterCharacter->GetActorLocation() + OtherSide->ArrowComponent->GetForwardVector()
 		);
 
 	HamsterCharacter->SetActorRotation(PipeRotator);
 
	//Rotate the camera if desired. This bool is set in the editor.
 	if (ShouldRotateCamera) PlayerPawn->SpringArm->SetWorldRotation(FRotator(PlayerPawn->SpringArm->GetComponentRotation().Pitch, OtherSide->OutCameraAngle, PlayerPawn->SpringArm->GetComponentRotation().Roll));
 
	//We call this function after teleporting since it highlights the possible tiles. 
 	HamsterCharacter->CheckPossibleTiles();
 
	//Remove the teleport dynamic after teleporting if AutoTeleport was used.
 	if (AutoTeleport)
 	{
 		GameState->OnTurnEnded.RemoveAll(this);
 	}

	HamsterCharacter->HasUsedPipe = true;

	if (ShouldResetAutoTeleport)
	{
		AutoTeleport = false;
		ShouldResetAutoTeleport = false;
	}

	HamsterCharacter->IsMoving = false;

	PlayerController->CanMoveCamera = true;

	//We relock the camera onto the hamster after teleporting. It prevents the player from losing its character.
	PlayerController->DisableFreeCamera();
}

void ADHPipe::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ADHPipe::OnBeginOverlap);
	
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ADHPipe::OnEndOverlap);
	
	//Fetching required actors.
	HamsterCharacter = Cast<ADHHamsterCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ADHHamsterCharacter::StaticClass()));
	GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));
	PlayerController = Cast<ADHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void ADHPipe::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//The player is on the adjacent tile.
	if (OtherActor->IsA(ADHHamsterCharacter::StaticClass()))
	{
		//The player already clicked on the pipe. We will teleport the hamster when the turn ends.
		if (AutoTeleport)
		{
			GameState->OnTurnEnded.AddDynamic(this, &ADHPipe::UsePipe);
		}
		//Enable pipe use.
		else
		{
			CanUsePipe = true;
		}
		
	}
}

void ADHPipe::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	//If the hamster leaves the adjacent tile, disable the use of the pipe.
	if (OtherActor->IsA(ADHHamsterCharacter::StaticClass()))
	{
		CanUsePipe = false;
	}
}

