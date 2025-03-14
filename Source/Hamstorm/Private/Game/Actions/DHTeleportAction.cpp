// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Actions/DHTeleportAction.h"

#include "Environment/DHPipe.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DHPlayerController.h"
#include "Player/DHPlayerPawn.h"

//This function restores the state of the hamster to what it was before the teleportation.
void UDHTeleportAction::RevertAction()
{
	auto* PlayerController = Cast<ADHPlayerController>(UGameplayStatics::GetPlayerController(TeleportedActor->GetWorld(), 0));

	PlayerController->DisableFreeCamera();

	auto* PlayerPawn = Cast<ADHPlayerPawn>(PlayerController->GetPawn());

	//Repositions the camera
	PlayerPawn->SpringArm->SetWorldRotation(
		FRotator(PlayerPawn->SpringArm->GetComponentRotation().Pitch,
			InitialPipe->OutCameraAngle,
			PlayerPawn->SpringArm->GetComponentRotation().Roll)
		);
	
	TeleportedActor->SetActorTransform(OldTransform, false, nullptr, ETeleportType::ResetPhysics);
}
